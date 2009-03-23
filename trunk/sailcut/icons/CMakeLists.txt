include(IconUtilities)

generate_icns(icns sailcut.svg)
generate_ico(ico sailcut.svg)
generate_xpm(xpm panel.svg panelgroup.svg point.svg sailcut.svg)

add_custom_target(icons DEPENDS ${icns} ${ico} ${xpm})
