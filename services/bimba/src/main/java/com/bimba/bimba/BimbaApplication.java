package com.bimba.bimba;

import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.boot.autoconfigure.domain.EntityScan;
import org.springframework.context.annotation.ComponentScan;
import org.springframework.data.jpa.repository.config.EnableJpaRepositories;
import org.springframework.scheduling.annotation.EnableScheduling;

@SpringBootApplication
@ComponentScan(basePackages = "com.bimba.bimba")
@EntityScan("com.bimba.bimba.models")
@EnableJpaRepositories("com.bimba.bimba.repository")
@EnableScheduling
public class BimbaApplication {

	public static void main(String[] args) {
		SpringApplication.run(BimbaApplication.class, args);
	}

}
