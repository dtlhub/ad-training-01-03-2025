package com.bimba.bimba.security.jwt;

import java.nio.file.Files;
import java.nio.file.Paths;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class JwtConfig {
    @Bean
    public static String jwtSecret() {
    try {
        return Files.readString(Paths.get("/app/config/jwt_secret.txt")).trim();
    } catch (Exception e) {
        throw new RuntimeException("Failed to read JWT secret file", e);
    }
}
}
