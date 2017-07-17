#
# Copyright (c) 2014, The Linux Foundation. All rights reserved.
#
#  Permission to use, copy, modify, and/or distribute this software for any
#  purpose with or without fee is hereby granted, provided that the above
#  copyright notice and this permission notice appear in all copies.
#
#  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
#  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
#  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
#  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
#  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
#  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
#  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
#

if [ "$ACTION" = "released" -a "$BUTTON" = "wps" ]; then
	[ -r /var/run/wifi-wps-enhc-extn.conf ] && exit 0
	echo "" > /dev/console


if [ "$SEEN" -lt 1 ]
then
	echo "WPS PUSH BUTTON EVENT DETECTED" > /dev/console
	
	
	for dir in /var/run/hostapd-*; do
		[ -d "$dir" ] || continue
		for vap_dir in $dir/ath* $dir/wlan*; do
		[ -r "$vap_dir" ] || continue
		nopbn=`iwpriv "${vap_dir#"$dir/"}"   get_nopbn  |   cut -d':' -f2`
		if [ $nopbn != 1 ]; then
			hostapd_cli -i "${vap_dir#"$dir/"}" -p "$dir" wps_pbc
		fi
		done
	done



        elif [ "$SEEN" -gt 5 ]; then
                echo "" > /dev/console
                echo "RESET TO FACTORY SETTING EVENT DETECTED" > /dev/console
                echo "PLEASE WAIT WHILE REBOOTING THE DEVICE..." > /dev/console
	         jffs2reset  -y && reboot &

	fi


fi
