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

# Use environment variable for JWT secret
ENV JWT_SECRET=default_secret_replace_in_production

ENTRYPOINT ["java", "-jar", "app.jar", \
           "--bezkoder.app.jwtSecret=${JWT_SECRET}"]
