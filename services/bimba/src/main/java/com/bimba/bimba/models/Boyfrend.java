package com.bimba.bimba.models;
import jakarta.validation.constraints.NotBlank;

public class Boyfrend {
    @NotBlank
    private String name = "masik";

    @NotBlank
    private String age = "65";

    @NotBlank
    private Integer money = 1000000;

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public String getAge() {
        return age;
    }

    public void setAge(String age) {
        this.age = age;
    }

    public Integer getMoney() {
        return money;
    }

    public void setMoney(Integer money) {
        this.money = money;
    }
}
