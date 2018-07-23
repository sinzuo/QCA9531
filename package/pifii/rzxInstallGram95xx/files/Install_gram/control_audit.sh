#!/bin/sh

GRAM_APPS_PATH="/tmp/apps"
GRAM_SCRIP_PATH="${GRAM_APPS_PATH}/gram/run_env/script"

#start_c=${0:0:1}
start_c=`echo $0 | sed -r 's/^(.{1})(.+)/\1/g'`
if [ "$start_c" = "/" ] ; then
    dir=${0%/*}
    if [ "$dir" = "" ] ; then
        dir="/"
    fi
elif [ "$start_c" = "." ]; then
# tmp=${0:2}
tmp=`echo $0 | sed -r 's/^(.{2})(.+)/\2/g'`
    val=`echo $tmp | grep -c "/"`
    if [ $val -eq 0 ] ; then
        tmp=""
    else
        tmp=${tmp%/*}
    fi
    dir=`pwd`"/"${tmp}
else
    val=`echo $0 | grep -c "/"`
    if [ $val -eq 0 ] ; then
        tmp=""
    else
        tmp=${0%/*}
    fi
    dir=`pwd`"/"${tmp}
fi

show_help(){                                                                                              
    echo "Usage: $0"                                                                          
    echo "  $0 --start"                                                                         
    echo "  $0 --stop"                                                     
    echo "  $0 --restart"   
    echo "  $0 --stop_clean"   
    exit                                                                                      
} 

stop_all()
{
    killall -9 InstallGramDog.sh 1>/dev/null 2>/dev/null
    killall -9 InstallGram.exe 1>/dev/null 2>/dev/null

    cd $GRAM_SCRIP_PATH
    ./stop_gram.sh 1>/dev/null 2>/dev/null
}

start_all()
{
    cd $dir
    ./InstallGramDog.sh 1>/dev/null 2>/dev/null &
}

restart_all()
{
    stop_all    
    sleep 10
    start_all
}
                                                                                 
clean_gram()
{
    rm -rf /tmp/gram
    rm -rf ${GRAM_APPS_PATH}/*
}

if [ $# -eq 0 ];then  
    show_help
    exit 0                                                                          
fi  

case "$1" in                                                                              
        --start) start_all ;;                                                     
        --stop) stop_all ;;
        --restart) restart_all ;;
        --stop_clean)
            stop_all
            clean_gram
            ;;
        *) show_help ;;                                                          
esac 

exit 0
