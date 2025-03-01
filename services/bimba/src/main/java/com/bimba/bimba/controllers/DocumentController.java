package com.bimba.bimba.controllers;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.UUID;
import java.sql.Date;
import java.time.LocalDateTime;

import javax.xml.parsers.ParserConfigurationException;

import org.springframework.security.core.context.SecurityContextHolder;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.ContentDisposition;
import org.springframework.http.HttpHeaders;
import org.springframework.http.MediaType;
import com.bimba.bimba.payloads.request.DocumentUploadRequest;
import com.bimba.bimba.payloads.response.DocumentResponse;

import org.springframework.http.ResponseEntity;
import org.xml.sax.SAXException;
import org.springframework.web.bind.annotation.RequestMapping;
import com.bimba.bimba.models.User;
import com.bimba.bimba.models.Boyfrend;
import com.bimba.bimba.models.Document;
import com.bimba.bimba.repository.DocumentRepository;
import com.bimba.bimba.security.services.UserDetailsImpl;
import com.bimba.bimba.services.DocumentService;
import com.fasterxml.jackson.databind.ObjectMapper;

import org.springframework.web.servlet.ModelAndView;

import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.multipart.MultipartFile;
import org.springframework.http.HttpStatus;


@RestController
@RequestMapping("/document")
public class DocumentController {

    @Autowired
    DocumentService documentService;
    @Autowired
    private ObjectMapper objectMapper;

    @GetMapping("/")
    public ModelAndView getDocuments() {
        List<Document> documents = documentService.getUserDocuments();
        Map<String, Object> params = new HashMap<String, Object>();
        params.put("documents", documents);
        return new ModelAndView("documents", params);
    }

    @GetMapping("/download/{uuid}")
    public ResponseEntity<?> get(@PathVariable UUID uuid) throws ParserConfigurationException, SAXException, IOException {
        byte[] content = documentService.getDocument(uuid.toString());
        HttpHeaders httpHeaders = new HttpHeaders();
        httpHeaders.set(HttpHeaders.CONTENT_TYPE, MediaType.APPLICATION_OCTET_STREAM_VALUE);
        httpHeaders.set(HttpHeaders.CONTENT_DISPOSITION, ContentDisposition.attachment().filename(String.format("report-%s.txt", uuid.toString())).build().toString());
        return ResponseEntity.ok().headers(httpHeaders).body(content);
    }

    @GetMapping("/search")
    public ModelAndView getMethodName(@RequestParam String name) {
        List<Document> documents = new ArrayList<>();
        if (name.isEmpty()) {
            documents = documentService.getUserDocuments();
        } else {
            documents = documentService.searchDocuments(name);
        }
        Map<String, Object> params = new HashMap<String, Object>();
        params.put("documents", documents);
        return new ModelAndView("documents", params);
    }

    @PostMapping("/upload")
    public ResponseEntity<?> uploadDocument(
            @RequestParam("file") MultipartFile file,
            @RequestParam("name") String name,
            @RequestParam("object") String object) {
        try {
            String uuid = UUID.randomUUID().toString();
            Boyfrend boyfrend = objectMapper.readValue(object, Boyfrend.class);
            Document document = new Document(
                uuid,
                null,
                name,
                uuid + ".docx",
                LocalDateTime.now()
            );

            documentService.uploadDocument(document, file, boyfrend);

            return ResponseEntity.ok(new DocumentResponse(uuid, "File uploaded successfully!"));
        } catch (Exception e) {
            return ResponseEntity
                .status(HttpStatus.INTERNAL_SERVER_ERROR)
                .body("Failed to upload file: " + e.getMessage());
        }
    }

}
