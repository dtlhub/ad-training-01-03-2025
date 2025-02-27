# Build stage
FROM maven:3.8.4-openjdk-17 as builder
WORKDIR /app
COPY pom.xml .
COPY src ./src
RUN mvn clean package -DskipTests

# Run stage
FROM openjdk:17-slim
WORKDIR /app
COPY --from=builder /app/target/*.jar app.jar

# Create directory for file uploads
RUN mkdir -p /root/uploads && \
    chmod 777 /root/uploads

EXPOSE 8070
ENTRYPOINT ["java","-jar","app.jar"]
