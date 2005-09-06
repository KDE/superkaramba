#!/bin/sh
themes=(
	"text/text.theme"
	"autoHide/main.theme" 
	"bar/bar.theme"
	"graph/graph.theme"
	"image/image.theme"
	"popupMenu/popupMenu.theme"
	"taskBar/cleanbar/cleanbar.theme"
	"richtext/richtext.theme"
	"richtext/rtext.theme"
	"globalMouse/eyes.theme"
	"unicode/unicode.theme"
        "mouseDrag/karmix/karmix.theme"
        "input_api/input_api.theme"
        "input_example/input_example.theme"
)

for theme in "${themes[@]}"
do
	echo $theme
	dcop `dcop superkaramba* | head -n 1` default openTheme $PWD/$theme
	read a
	dcop `dcop superkaramba* | head -n 1` default closeTheme `expr $theme : '.*/\(.*\)\.'`
done

#dcop `dcop superkaramba* | head -n 1` default quit
