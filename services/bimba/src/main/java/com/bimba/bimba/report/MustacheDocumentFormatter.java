package com.bimba.bimba.report;

import fr.opensagres.xdocreport.template.formatter.AbstractDocumentFormatter;
import fr.opensagres.xdocreport.template.formatter.DirectivesStack;

public class MustacheDocumentFormatter extends AbstractDocumentFormatter {
    @Override
    public String formatAsFieldItemList(String content, String fieldName, boolean forceAsField) {
        if (fieldName.startsWith("___")) {
            return "${" + fieldName + "}";
        }
        return "{{" + fieldName + "}}";
    }

    @Override
    public String getStartLoopDirective(String itemNameList, String listName) {
        return "{{#" + listName + "}}";
    }

    @Override
    public String getEndLoopDirective(String itemNameList) {
        return "{{/" + itemNameList + "}}";
    }

    @Override
    public String getLoopCountDirective(String fieldName) {
        return "{{" + fieldName + ".size}}";
    }

    @Override
    public String getStartIfDirective(String fieldName, boolean exists) {
        if (fieldName.startsWith("___")) {
            return exists ? "${#if " + fieldName + "}" : "${#if !(" + fieldName + ")}";
        }
        return exists ? "{{#" + fieldName + "}}" : "{{^" + fieldName + "}}";
    }

    @Override
    public String getEndIfDirective(String fieldName) {
        String baseFieldName = fieldName;
        int dotIndex = fieldName.indexOf('.');
        if (dotIndex > 0) {
            baseFieldName = fieldName.substring(0, dotIndex);
        }

        if (baseFieldName.startsWith("___")) {
            return "${/if}";
        }
        return "{{/" + baseFieldName + "}}";
    }

    @Override
    public String formatAsSimpleField(boolean noescape, boolean encloseInDirective, String... fields) {
        StringBuilder result = new StringBuilder();
        for (String field : fields) {
            result.append(noescape ? "{{{" : "{{").append(field).append(noescape ? "}}}" : "}}");
        }
        return result.toString();
    }

    @Override
    public boolean containsInterpolation(String content) {
        if (content == null) return false;
        return content.contains("{{") || content.contains("}}") ||
               content.contains("${") || content.contains("}");
    }

    @Override
    public int extractListDirectiveInfo(String content, DirectivesStack directives,
            boolean dontRemoveListDirectiveInfo) {
        if (content == null) return -1;
        return content.indexOf("{{#");
    }

    @Override
    public String extractModelTokenPrefix(String newContent) {
        return "{{";
    }

    @Override
    public int getIndexOfScript(String fieldName) {
        return -1;
    }

    @Override
    public String getFunctionDirective(boolean noescape, boolean encloseInDirective, String key, String methodName,
            String... parameters) {
        StringBuilder result = new StringBuilder();
        result.append(noescape ? "{{{" : "{{").append(key);
        if (parameters != null && parameters.length > 0) {
            for (String param : parameters) {
                result.append(" ").append(param);
            }
        }
        result.append(noescape ? "}}}" : "}}");
        return result.toString();
    }

    @Override
    public boolean hasDirective(String characters) {
        return characters != null && characters.contains("{{");
    }

    @Override
    public String formatAsCallTextStyling(long variableIndex, String fieldName, String documentKind, String syntaxKind,
            boolean syntaxWithDirective, String elementId, String entryName) {
        return "{{" + fieldName + "}}";
    }

    @Override
    public String formatAsTextStylingField(long variableIndex, String textBeforeProperty) {
        return "{{" + textBeforeProperty + "}}";
    }

    @Override
    public String getElseDirective() {
        return "{{^}}";
    }

    @Override
    public String getSetDirective(String name, String value, boolean valueIsField) {
        if (valueIsField) {
            return "{{#set " + name + "=" + value + "}}";
        }
        return "{{#set " + name + "=\"" + value + "\"}}";
    }

    @Override
    public String getStartNoParse() {
        return "{{=<% %>=}}";
    }

    @Override
    public String getEndNoParse() {
        return "<%={{ }}=%>";
    }

    @Override
    public String getDefineDirective(String name, String value) {
        return "{{#define " + name + "}}" + value + "{{/define}}";
    }

    @Override
    public boolean isInstruction(String tagContent) {
        if (tagContent == null) return false;
        if (tagContent.startsWith("${")) {
            return tagContent.contains("#if") || tagContent.contains("/if");
        }
        return tagContent.startsWith("#") ||
               tagContent.startsWith("/") ||
               tagContent.startsWith("^") ||
               tagContent.startsWith(">");
    }

    @Override
    protected String getItemToken() {
        return ".";
    }

    @Override
    protected boolean isModelField(String content, String fieldName) {
        return content != null && content.contains("{{" + fieldName + "}}");
    }
}

