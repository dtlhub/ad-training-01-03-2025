# Build stage
FROM maven:3.8.4-openjdk-17 as builder
WORKDIR /app
COPY pom.xml .
COPY src ./src
RUN mvn clean package -D skipTests

# Run stage
FROM openjdk:17-slim
WORKDIR /app
COPY --from=builder /app/target/*.jar app.jar
#RUN sudo apt install libssl-dev
# Create directory for file uploads
RUN mkdir -p /root/uploads && \
    chmod 777 /root/uploads &&\
    mkdir -p /app/config && \
    openssl rand -hex 64 > /app/config/jwt_secret.txt && \
    chmod 600 /app/config/jwt_secret.txt

EXPOSE 8070

ENTRYPOINT ["java","-jar","app.jar"]
