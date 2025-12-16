#!/system/bin/sh
sleep 2
ui_print "Author:"
ui_print "Telegram: @RiProG | Channel: @RiOpSo | Group: @RiOpSoDisc"
ui_print "Support me on Sociabuzz:"
ui_print "http://sociabuzz.com/RiProG/tribe"
sleep 2
if [ ! -z "$AXERON" ]; then
	ui_print "----------------------------------------"
	ui_print "⚠️  WARNING:"
	ui_print "- AXERON mode detected."
	ui_print "- This module runs with SU privileges."
	ui_print "- Module will disable or remove itself automatically after reboot."
	ui_print "----------------------------------------"
	sleep 2
fi
ui_print "Extracting module files"
architecture=$(uname -m)
if [ -f "$MODPATH/system/bin/BKT" ]; then
	ui_print "Default binary detected. Skipping architecture detection."
else
	ui_print "- Detected architecture: $architecture"
	case "$architecture" in
	armv7l | armv8l)
		ui_print "- Architecture $architecture is supported."
		ui_print "- Installation continues."
		cp "$MODPATH/system/bin/BKT_arm" "$MODPATH/system/bin/BKT"
		;;
	aarch64)
		ui_print "- Architecture $architecture is supported."
		ui_print "- Installation continues."
		cp "$MODPATH/system/bin/BKT_arm64" "$MODPATH/system/bin/BKT"
		;;
	*)
		ui_print "- Architecture $architecture is not supported."
		ui_print "- Installation aborted."
		rm -rf "$MODPATH"
		exit 1
		;;
	esac
fi
rm "$MODPATH/system/bin/BKT_arm"
rm "$MODPATH/system/bin/BKT_arm64"
chmod +x "$MODPATH/system/bin/BKT"
