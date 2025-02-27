package com.bimba.bimba.controllers;

import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.servlet.ModelAndView;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RequestParam;


@RestController
@RequestMapping("/")
public class IndexController {


    @GetMapping("/")
    public ModelAndView getIndexPage(){
        return new ModelAndView("index");
}
}
