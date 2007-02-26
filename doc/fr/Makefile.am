include $(top_srcdir)/Makefile.common

FIGS = coordinates_system.fig sailplan.fig sail_seams.fig sail_edges.fig develop_panel_drawing.fig head_gores_definition.fig

htmldir = $(pkgdocdir)/html/fr
dist_html_DATA = index.html $(FIGS:.fig=.png)

EXTRA_DIST = index.docbook $(FIGS)
MAINTAINERCLEANFILES = $(dist_html_DATA)
