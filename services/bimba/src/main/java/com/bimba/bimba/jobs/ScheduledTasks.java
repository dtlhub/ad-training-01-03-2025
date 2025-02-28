package com.bimba.bimba.jobs;

import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;
import com.bimba.bimba.services.DocumentService;
import org.springframework.beans.factory.annotation.Autowired;

@Component
public class ScheduledTasks {
    @Autowired
    private DocumentService fileCleanerService;

    @Scheduled(fixedRate = 300000)
    public void scheduleFileCleaningTask() {
        fileCleanerService.cleanOldDocuments();
    }
}

