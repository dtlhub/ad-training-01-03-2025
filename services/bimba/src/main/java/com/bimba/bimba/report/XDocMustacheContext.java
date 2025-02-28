package com.bimba.bimba.report;

import java.util.HashMap;
import java.util.Map;

import fr.opensagres.xdocreport.template.IContext;
import fr.opensagres.xdocreport.template.utils.TemplateUtils;

public class XDocMustacheContext implements IContext {
    public static final long serialVersionUID = 1L;

    private final Map<String, Object> map;

    public XDocMustacheContext() {
        this(new HashMap<String, Object>());
    }

    public XDocMustacheContext(Map<String, Object> contextMap) {
        map = contextMap;
    }

    public Object put(String key, Object value) {
        Object result = TemplateUtils.putContextForDottedKey(this, key, value);
        if (result == null) {
            return map.put(key, value);
        }
        return result;
    }

    public Object get(String key) {
        return map.get(key);
    }

    public void putMap(Map<String, Object> contextMap) {
        map.putAll(contextMap);
    }

    public Map<String, Object> getContextMap() {
        return map;
    }
}


