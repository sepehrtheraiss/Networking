FROM python:2.7
WORKDIR /app
COPY . /app
EXPOSE 4200:8080 8081:8081/udp 8082:8082/udp
ENV NAME UDP
CMD ["python","udp_main.py"]
