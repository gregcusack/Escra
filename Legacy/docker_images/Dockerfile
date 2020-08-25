FROM ubuntu:latest
ADD run.sh /tmp/run.sh
RUN chmod +x /tmp/run.sh
RUN apt-get update
RUN apt-get -y install curl && curl -s https://packagecloud.io/install/repositories/akopytov/sysbench/script.deb.sh |  bash && apt -y install sysbench
ENTRYPOINT ["/tmp/run.sh"]

