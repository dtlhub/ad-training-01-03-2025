FROM ubuntu:24.04

COPY cleaner.sh /opt/cleaner.sh
RUN chmod +x /opt/cleaner.sh

# RUN useradd --no-create-home --shell /bin/false --uid 1000 --user-group cleaner
# USER cleaner

ENTRYPOINT ["/opt/cleaner.sh"]