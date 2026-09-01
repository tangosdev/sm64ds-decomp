# Same three-line shape as port-ingest/Dockerfile, on purpose: one stdlib-only
# python file, no build step, no package manager, nothing to pin and nothing to
# audit. The service has no dependencies because it has no disk, no database,
# no credential and no outbound request.
FROM python:3.12-alpine
COPY app/server.py /app/
EXPOSE 8091
ENTRYPOINT ["python3", "/app/server.py"]
