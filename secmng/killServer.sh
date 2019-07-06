#! /bin/bash
#查询当前用户 whoami `` $()
user=`whoami`

#sed-行 awk-列 '{print $1}'
mypid=`ps -u ${user} | grep keymngserver | awk '{print $1}'`

#zero 是否为零
if [ -z ${mypid} ]; then
	echo "The process was not been started."
	exit 1
fi

kill -10 ${mypid}
echo "Kill keymngserver successful!"
