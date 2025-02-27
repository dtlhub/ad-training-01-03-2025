package com.bimba.bimba;

import org.junit.jupiter.api.Test;
import org.springframework.boot.test.context.SpringBootTest;
import org.springframework.context.annotation.Import;
import org.springframework.test.context.ActiveProfiles;
import com.bimba.bimba.config.TestSecurityConfig;
import com.bimba.bimba.config.TestUserDetailsService;
import com.bimba.bimba.config.TestJwtUtils;

@SpringBootTest(webEnvironment = SpringBootTest.WebEnvironment.RANDOM_PORT)
@ActiveProfiles("test")
@Import({TestSecurityConfig.class, TestUserDetailsService.class, TestJwtUtils.class})
class BimbaApplicationTests {

	@Test
	void contextLoads() {
	}

}
