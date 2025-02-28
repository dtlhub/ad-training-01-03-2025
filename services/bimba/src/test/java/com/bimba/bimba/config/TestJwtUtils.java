package com.bimba.bimba.config;

import org.springframework.boot.test.context.TestConfiguration;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Primary;
import com.bimba.bimba.security.jwt.JwtUtils;

@TestConfiguration
public class TestJwtUtils {
    
    @Bean
    @Primary
    public JwtUtils jwtUtils() {
        return new JwtUtils();
    }
} 