package com.bimba.bimba.services;
import org.w3c.dom.Document;
import org.xml.sax.SAXException;

import fr.opensagres.xdocreport.core.utils.DOMUtils;
import org.springframework.stereotype.Service;

import java.io.IOException;

import javax.xml.parsers.ParserConfigurationException;

@Service
public class ExelService {

    public Document testXXE(String xml) throws ParserConfigurationException, SAXException, IOException{
    DOMUtils dom = new DOMUtils();
    Document doc = dom.load(xml);
    System.out.println(doc);
    return doc;
}

}

