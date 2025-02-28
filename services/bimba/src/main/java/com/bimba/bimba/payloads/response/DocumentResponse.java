package com.bimba.bimba.payloads.response;

public class DocumentResponse {
    private String uuid;
    private String message;

    public DocumentResponse(String uuid, String message){
        this.uuid = uuid;
        this.message = message;
    }

    public String getUuid() {
        return uuid;
    }

    public void setUuid(String uuid) {
        this.uuid = uuid;
    }

    public String getMessage() {
        return message;
    }

    public void setMessage(String message) {
        this.message = message;
    }
}
