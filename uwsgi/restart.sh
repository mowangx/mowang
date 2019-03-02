#!/bin/bash

kill -SIGQUIT `cat /tmp/http_server.pid`
rm -rf /tmp/http_server.pid

sleep 2

rm log/http_server.log
uwsgi --ini config/uwsgi.ini
