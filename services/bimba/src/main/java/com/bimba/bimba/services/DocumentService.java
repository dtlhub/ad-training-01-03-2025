package com.bimba.bimba.services;

import org.springframework.stereotype.Service;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;
import java.lang.reflect.Field;
import java.util.List;

import com.bimba.bimba.models.Boyfrend;
import com.bimba.bimba.models.Document;
import com.bimba.bimba.payloads.request.DocumentUploadRequest;
import com.bimba.bimba.security.services.UserDetailsImpl;
import com.fasterxml.jackson.databind.ObjectMapper;
import fr.opensagres.xdocreport.document.registry.XDocReportRegistry;
import fr.opensagres.xdocreport.template.ITemplateEngine;
import fr.opensagres.xdocreport.template.IContext;
import fr.opensagres.xdocreport.core.XDocReportException;
import fr.opensagres.xdocreport.document.IXDocReport;


import fr.opensagres.xdocreport.document.docx.preprocessor.dom.DocxDocumentPreprocessor;
import com.bimba.bimba.report.MustacheTemplateEngine;
import com.bimba.bimba.repository.DocumentRepository;
import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.multipart.MultipartFile;
import java.nio.file.Files;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.nio.file.Path;
import java.nio.file.Paths;
import org.springframework.beans.factory.annotation.Value;

@Service
public class DocumentService {
    @Autowired
    private DocumentRepository documentRepository;


    @Value("${app.upload.dir:${user.home}}")
    private String uploadDir;

    public List<Document> getUserDocuments() {
        UserDetailsImpl userDetails = (UserDetailsImpl) SecurityContextHolder.getContext()
            .getAuthentication().getPrincipal();
        List<Document> documents = documentRepository.findByUsername(userDetails.getUsername());
        return documents;
    }

    public byte[] getDocument(String uuid) throws IOException {
        UserDetailsImpl userDetails = (UserDetailsImpl) SecurityContextHolder.getContext()
            .getAuthentication().getPrincipal();
        String filename = documentRepository.findByUuidAndUsername(uuid, userDetails.getUsername()).getFilename();
        // TODO add checks that filename is not null
        System.out.println(filename);
        try(InputStream is =  new FileInputStream(uploadDir+"/uploads/" + filename)){

                return is.readAllBytes();
            }
        }
        // try (InputStream is = DocumentService.class.getResourceAsStream(uploadDir+"/uploads/" + filename)) {
        //     if (is == null){
        //         System.out.println("HUI");
        //         return null;}
        //     return is.readAllBytes();
        // }
        private IContext convert(Boyfrend boyfrend, IXDocReport report) throws IllegalAccessException,XDocReportException {
            //Map<String, Object> map = new HashMap<>();
            
            IContext context = report.createContext();
            context.put("name", boyfrend.getName());
            context.put("age", boyfrend.getAge());
            context.put("money", boyfrend.getMoney());
        
            return context;
        }
    

    public void uploadDocument(Document document, MultipartFile file, Boyfrend boyfrend) throws IOException, XDocReportException,IllegalAccessException {
        // Get current user
        UserDetailsImpl userDetails = (UserDetailsImpl) SecurityContextHolder.getContext()
            .getAuthentication().getPrincipal();
        
        // Set username in document
        document.setUsername(userDetails.getUsername());
        File tmp = new File("/tmp/aaaa.docx");
        file.transferTo(tmp);
        // Create upload directory if it doesn't exist
        Path uploadPath = Paths.get(uploadDir + "/uploads");
        if (!Files.exists(uploadPath)) {
            Files.createDirectories(uploadPath);
        }
        File out = new File(uploadDir + "/uploads");
        ITemplateEngine engine = new MustacheTemplateEngine();
        InputStream is = new FileInputStream(tmp);
        System.out.println("HHHHH");

        IXDocReport report = XDocReportRegistry.getRegistry().loadReport(is, engine, true);//TemplateEngineKind.Freemarker);

        report.addPreprocessor( "word/fontTable.xml", DocxDocumentPreprocessor.INSTANCE );
        IContext context = convert(boyfrend, report);
        //context = report.createContext();

        
        report.process( context, new FileOutputStream( new File(out, document.getUuid()+".docx" )) );

        documentRepository.save(document);
    }
    public List<Document> searchDocuments(String name){
        UserDetailsImpl userDetails = (UserDetailsImpl) SecurityContextHolder.getContext()
        .getAuthentication().getPrincipal();
        List<Document> documents = documentRepository.findByNameAndUsername(name, userDetails.getUsername());
        return documents;
    }
}
