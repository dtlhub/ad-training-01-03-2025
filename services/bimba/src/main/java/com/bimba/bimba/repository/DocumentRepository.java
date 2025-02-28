package com.bimba.bimba.repository;

import java.time.LocalDateTime;
import java.util.List;

import org.springframework.data.jpa.repository.JpaRepository;
import org.springframework.data.jpa.repository.Query;

import com.bimba.bimba.models.Document;

public interface DocumentRepository extends JpaRepository<Document, Long> {
    List<Document> findByUsername(String username);
    
    Document findByUuidAndUsername(String uuid, String username);
    List<Document> findByNameAndUsername(String name, String username);

    @Query("SELECT f FROM Document f WHERE f.date <= :timeStamp")
    List<Document> findOldFiles(LocalDateTime timeStamp);
   }
