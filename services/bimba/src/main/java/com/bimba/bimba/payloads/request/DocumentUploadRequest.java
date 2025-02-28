package com.bimba.bimba.payloads.request;

import com.bimba.bimba.models.Boyfrend;

import jakarta.validation.constraints.NotBlank;

public class DocumentUploadRequest {
    @NotBlank
    private Boyfrend[] boyfrends;
}
