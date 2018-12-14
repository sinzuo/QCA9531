#!/bin/sh
# Copyright (C) 2011-2012 Luka Perkov <freecwmp@lukaperkov.net>

. /lib/functions.sh
#. /usr/share/libubox/jshn.sh
#. /usr/share/shflags/shflags.sh
#. /usr/share/freecwmp/defaults

usage()
{
cat <<EOF
USAGE: /usr/sbin/freecwmp [flags] command [parameter] [values]
Command:
  get [value|notification|tags|all]
  set [value|notification|tag]
  download
  factory_reset
  reboot

Flags:
  -n,--[no]newline:  do not output the trailing newline (default: false)
  -v,--[no]value:  output values only (default: false)
  -e,--[no]empty:  output empty parameters (default: false)
  -l,--[no]last:  output only last line ; for parameters that tend to have huge output
                  (default: false)
  -d,--[no]debug:  give debug output (default: false)
  -D,--[no]dummy:  echo system commands (default: false)
  -f,--[no]force:  force getting values for certain parameters (default: false)
  -u,--url:  file to download [download only] (default: '')
  -s,--size:  size of file to download [download only] (default: '')
  -m,--md5:  md5code of file to download [download only] (default: '')
  -h,--[no]help:  show this help (default: false)
EOF
exit 1
}

FLAGS_TRUE=0                                                                  
FLAGS_FALSE=1                                                                 
FLAGS_ERROR=2

#default value
FLAGS_newline=$FLAGS_FALSE
FLAGS_value=$FLAGS_FALSE
FLAGS_empty=$FLAGS_FALSE
FLAGS_last=$FLAGS_FALSE
FLAGS_debug=$FLAGS_FALSE
FLAGS_dummy=$FLAGS_FALSE 
FLAGS_force=$FLAGS_FALSE

ARGS=`getopt -a -o nveldDfu:s:m:h -l newline,value,empty,last,debug,dummy,force,url:,size:,md5:,help -- "$@"`  
[ $? -ne 0 ] && usage  
#set -- "${ARGS}"  
eval set -- "${ARGS}" 
 
while true  
do  
        case "$1" in 
        -n|--newline)  
                FLAGS_newline=$FLAGS_TRUE
                ;;  
        -v|--value)  
                FLAGS_value=$FLAGS_TRUE 
                ;;  
        -e|--empty)  
                FLAGS_empty=$FLAGS_TRUE
                ;;  
        -l|--last)  
                FLAGS_last=$FLAGS_TRUE
                ;;  
        -d|--debug)  
                FLAGS_debug=$FLAGS_TRUE
                ;;  
        -D|--dummy)  
                FLAGS_dummy=$FLAGS_TRUE 
                ;;  
        -f|--force)  
                FLAGS_force=$FLAGS_TRUE 
                ;;  
        -u|--url)  
                FLAGS_url="$2"
		shift
                ;;  
        -s|--size)  
                FLAGS_size="$2"
		shift
                ;;  
        -m|--md5)  
                FLAGS_md5="$2"
		shift
                ;;  
        -h|--help)  
                usage  
                ;;  
        --)  
                shift  
                break 
                ;;  
        esac  
shift  
done

if [ ${FLAGS_newline} -eq ${FLAGS_TRUE} ]; then
	ECHO_newline='-n'
fi

case "$1" in
	set)
		if [ "$2" = "notification" ]; then
			__arg1="$3"
			__arg2="$4"
			action="set_notification"
		elif [ "$2" = "tag" ]; then
			__arg1="$3"
			__arg2="$4"
			action="set_tag"
		elif [ "$2" = "value" ]; then
			__arg1="$3"
			__arg2="$4"
			action="set_value"
		else
			__arg1="$2"
			__arg2="$3"
			action="set_value"
		fi
		;;
	get)
		if [ "$2" = "notification" ]; then
			__arg1="$3"
			action="get_notification"
		elif [ "$2" = "tags" ]; then
			__arg1="$3"
			action="get_tags"
		elif [ "$2" = "value" ]; then
			__arg1="$3"
			action="get_value"
		elif [ "$2" = "all" ]; then
			__arg1="$3"
			action="get_all"
		else
			__arg1="$2"
			action="get_value"
		fi
		;;
	add)
		if [ "$2" = "object" ]; then
			__arg1="$3"
			action="add_object"
		fi
		;;
	download)
		action="download"
		;;
	factory_reset)
		action="factory_reset"
		;;
	reboot)
		action="reboot"
		;;
esac

if [ -z "$action" ]; then
	echo invalid action \'$1\'
	exit 1
fi

if [ ${FLAGS_debug} -eq ${FLAGS_TRUE} ]; then
	echo "[debug] started at \"`date`\""
fi

get_value_functions=""
set_value_functions=""
add_object_functions=""

load_script() {
	. $1 
}

load_function() {
	get_value_functions="$get_value_functions get_$1"
	set_value_functions="$set_value_functions set_$1"
	add_object_functions="$add_object_functions add_$1"
}

handle_scripts() {
	local section="$1"
	config_get prefix "$section" "prefix"
	config_list_foreach "$section" 'location' load_script
	config_list_foreach "$section" 'function' load_function
}

config_load freecwmp
config_foreach handle_scripts "scripts"

if [ "$action" = "get_value" -o "$action" = "get_all" ]; then
	if [ ${FLAGS_force} -eq ${FLAGS_FALSE} ]; then
		__tmp_arg="Device."
		# TODO: don't check only string length ; but this is only used
		#       for getting correct prefix of CWMP parameter anyway
		if [  ${#__arg1} -lt ${#__tmp_arg} ]; then
			echo "CWMP parameters usualy begin with 'InternetGatewayDevice.' or 'Device.'     "
			echo "if you want to force script execution with provided parameter use '-f' flag."
			exit -1
		fi
	fi
	for function_name in $get_value_functions
	do
		$function_name "$__arg1"
	done
fi

if [ "$action" = "set_value" ]; then
	for function_name in $set_value_functions
	do
		$function_name "$__arg1" "$__arg2"
	done
fi

if [ "$action" = "add_object" ]; then
	for function_name in $add_object_functions
	do
		$function_name "$__arg1"
	done
fi

if [ "$action" = "get_notification" -o "$action" = "get_all" ]; then
	freecwmp_get_parameter_notification "x_notification" "$__arg1"
	freecwmp_notification_output "$__arg1" "$x_notification"
fi

if [ "$action" = "set_notification" ]; then
	freecwmp_set_parameter_notification "$__arg1" "$__arg2"
	/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} commit
fi

if [ "$action" = "get_tags" -o "$action" = "get_all" ]; then
	freecwmp_get_parameter_tags "x_tags" "$__arg1"
	freecwmp_tags_output "$__arg1" "$x_tags"
fi

if [ "$action" = "set_tag" ]; then
	freecwmp_set_parameter_tag "$__arg1" "$__arg2"
	/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} commit
fi

if [ "$action" = "download" ]; then
        #firmware name check
        fp=""
        if [ -f /etc/version ];then
           fp=$(cat /etc/version)
        fi
        filename=${FLAGS_url##*/}
        isname=$(echo "$filename" | grep "^$fp")
        full="firmware_update.bin"
        isdef=$(echo "$filename" | grep "^$full$")
        if [ -z "$isname" -a -z "$isdef" ] ;then
            exit 1 
        fi
        #firmware name check end
	rm /tmp/freecwmp_download 2> /dev/null
	wget -O /tmp/freecwmp_download "${FLAGS_url}" > /dev/null 2>&1

	dl_size=`ls -l /tmp/freecwmp_download | awk '{ print $5 }'`
	dl_md5=`md5sum /tmp/freecwmp_download | awk '{print $1}'`
	if [ ! "$dl_size" -eq "${FLAGS_size}" -o "$dl_md5" != "${FLAGS_md5}" ]; then
		rm /tmp/freecwmp_download 2> /dev/null
		exit 2 
	fi
	sysupgrade -c /tmp/freecwmp_download &
fi

if [ "$action" = "factory_reset" ]; then
	if [ ${FLAGS_dummy} -eq ${FLAGS_TRUE} ]; then
		echo "# factory_reset"
	else
		#jffs2_mark_erase "rootfs_data"
		#sync
		jffs2reset -y &>/dev/null
		reboot
	fi
fi

if [ "$action" = "reboot" ]; then
	/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} set freecwmp.@local[0].event="boot"
	/sbin/uci -q ${UCI_CONFIG_DIR:+-c $UCI_CONFIG_DIR} commit

	if [ ${FLAGS_dummy} -eq ${FLAGS_TRUE} ]; then
		echo "# reboot"
	else
		sync
		reboot
	fi
fi

if [ ${FLAGS_debug} -eq ${FLAGS_TRUE} ]; then
	echo "[debug] exited at \"`date`\""
fi
