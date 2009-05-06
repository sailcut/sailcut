import getopt
import os
import re
import shutil

def copy_lib(base, lib, dest):
	sourcefile = os.path.join(base, lib)
	destfile = os.path.join(dest, lib)
	destdir = os.path.dirname(destfile)

	if os.path.exists(destfile):
		return False

	if not os.path.exists(sourcefile):
		raise Exception("Could not find %s" % sourcefile)

	if not os.path.exists(destdir):
		os.makedirs(destdir)
		
	sys.stderr.write(" copying %s\n" % lib)
	shutil.copy(sourcefile, destfile)
	if os.path.islink(sourcefile):
		target = os.readlink(sourcefile)
		if target.count("/"):
			raise Exception("Could not handle link to %s" % target)
		copy_lib(base, target, dest)

	return True

def get_type(bin):
	# cheap tests
	if re.match(".*\.(exe|dll)", bin):
		return "pe32"
	elif re.match(".*\.dylib", bin):
		return "mach-o"

	# magic numbers
	magic = file(bin, "rb").read(4)
	if magic == '\x7fELF':
		return "elf"
	elif magic == '\xce\xfa\xed\xfe':
		return "mach-o"

	return None

def get_elf_depends(bin):
	out = os.popen("objdump -p %s" % bin).read()
	for line in out.splitlines():
		m = re.match('^\s+NEEDED\s+(.*)', line)
		if m:
			name = m.group(1)
			yield name

def get_mach_o_depends(bin):
	out = os.popen("otool -l %s" % bin).read()
	for line in out.splitlines():
		m = re.match('^\s+name (.*) \(', line)
		if m:
			name = m.group(1)
			if not name.startswith("/usr/lib/") and not name.startswith("/System/Library/"):
				yield name

def get_pe32_depends(bin):
	out = os.popen("i586-mingw32msvc-objdump -p %s" % bin).read()
	for line in out.splitlines():
		m = re.match('^\s+DLL Name: (.*)', line)
		if m:
			name = m.group(1)
			if re.match(".*\.dll", name) and not re.match("(msvcrt|.*32)\.dll", name):
				yield name

def get_depends(bin, exclude, externals=[]):
	if not os.path.exists(bin):
		raise Exception("Could not find %s" % bin)
	type = get_type(bin)
	if type == "elf":
		for name in get_elf_depends(bin):
			lib = name
			externals.append(lib)
	elif type == "mach-o":
		for name in get_mach_o_depends(bin):
			if exclude and name.startswith(exclude):
				continue
			lib = name
			if not os.path.exists(lib):
				lib = os.path.join("/Library/Frameworks", name)
				if not os.path.exists(lib):
					continue
			if not externals.count(lib):
				externals.append(lib)
				if os.path.islink(lib):
					lib = os.path.join(os.path.dirname(lib), os.readlink(lib))
					externals.append(lib)
				get_depends(lib, exclude, externals)
	if type == "pe32":
		for name in get_pe32_depends(bin):
			lib = os.path.join("/usr/i586-mingw32msvc/bin", name)
			if not os.path.exists(lib):
				continue
			if not externals.count(lib):
				externals.append(lib)
				get_depends(lib, exclude, externals)
	return externals

def fix_depends(bin, cmakefile=None, exclude=None):
	if not os.path.exists(bin):
		raise Exception("Could not find %s" % bin)

	# get binary type
	type = get_type(bin)
	if not type:
		raise Exception("Unknown binary type %s" % bin)

	# figure out destination
	if type == "pe32":
		destination = 'bin'
	else:
		destination = 'lib'

	sys.stderr.write("Processing %s (%s)\n" % (bin, type))
	if cmakefile:
		lines = []
		if os.path.exists(cmakefile):
			lines = file(cmakefile).readlines()
		for shipped in get_depends(bin, exclude):
			line = "install(FILES %s DESTINATION %s)\n" % (shipped, destination)
			if not lines.count(line):
				lines.append(line)
		file(cmakefile, "w").write("".join(sorted(lines)))
		
	if type == "mach-o":
		for name in get_mach_o_depends(bin):
			if name.count("/") and (not exclude or not name.startswith(exclude)):
				sys.stderr.write(" making %s relative\n" % name)
				os.system("install_name_tool -change %s %s %s" % (name, os.path.basename(name), bin))

if __name__ == "__main__":
	import sys
	opts, args = getopt.getopt(sys.argv[1:], "c:x:")
	config = {}
	for k, v in opts:
		if k == "-c":
			config['cmakefile'] = v
		elif k == "-x":
			config['exclude'] = v

	if len(args) < 1:
		sys.stderr.write("Usage: shipdepends.py [options] <files>\n")
		sys.exit(1)

	for bin in args:
		fix_depends(bin, **config)

