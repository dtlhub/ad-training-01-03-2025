package com.bimba.bimba.report;
import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.io.StringWriter;
import java.io.Writer;
import java.util.Map;

import fr.opensagres.xdocreport.template.freemarker.FreemarkerDocumentFormatter;
import com.github.mustachejava.Mustache;
import com.github.mustachejava.DefaultMustacheFactory;
import com.github.mustachejava.MustacheFactory;
import java.io.InputStream;
import java.io.InputStreamReader;
import com.github.mustachejava.MustacheNotFoundException;
import fr.opensagres.xdocreport.document.docx.DocxReport;
import fr.opensagres.odfdom.converter.core.utils.IOUtils;
import fr.opensagres.xdocreport.core.XDocReportException;
import fr.opensagres.xdocreport.core.io.IEntryReaderProvider;
import fr.opensagres.xdocreport.template.AbstractTemplateEngine;
import fr.opensagres.xdocreport.template.FieldsExtractor;
import fr.opensagres.xdocreport.template.IContext;
import fr.opensagres.xdocreport.template.formatter.IDocumentFormatter;

public class MustacheTemplateEngine extends AbstractTemplateEngine {
    private boolean forceModifyReader = false;
    private MustacheDocumentFormatter formatter = new MustacheDocumentFormatter();

    protected void processNoCache(String templateName, IContext context, Reader reader, Writer writer)
            throws XDocReportException, IOException {
        MustacheFactory mf = new DefaultMustacheFactory();
        Mustache m = mf.compile(reader, templateName);
        m.execute(writer, context);
        process(templateName, context, writer);
    }

    protected void processWithCache(String templateName, IContext context, Writer writer)
            throws XDocReportException, IOException {
        process(templateName, context, writer);
    }

    @Override
    public void process(String name, IContext context, Writer writer) throws XDocReportException, IOException {
        System.out.println("NOOOOOO");
    }

    @Override
    public void process(String reportId, String entryName, IEntryReaderProvider readerProvider, Writer writer,
                        IContext context) throws XDocReportException, IOException {
        MustacheFactory mf = new DefaultMustacheFactory() {
            @Override
            public Reader getReader(String resourceName) {
                Reader is = readerProvider.getEntryReader(entryName);
                if (is == null) {
                    throw new MustacheNotFoundException(entryName);
                }
                return is;
            }
        };
        if (entryName.equals("word/document.xml")) {
            Mustache m = mf.compile(entryName);
            StringWriter writer1 = new StringWriter();
            m.execute(writer1, context.getContextMap()).flush();
            writer.write(writer1.toString());
            writer.flush();
            writer.close();
            System.out.println(writer1.toString());
        }
    }

    @Override
    public void extractFields(Reader reader, String templateName, FieldsExtractor extractor) throws XDocReportException {
    }

    @Override
    public String getKind() {
        return "mustache";
    }

    @Override
    public String getId() {
        return "Mustache_2.0.x";
    }

    @Override
    public IContext createContext() {
        return new XDocMustacheContext();
    }

    @Override
    public IContext createContext(Map<String, Object> contextMap) {
        return new XDocMustacheContext(contextMap);
    }

    @Override
    public IDocumentFormatter getDocumentFormatter() {
        return formatter;
    }

    private Reader getReader(Reader reader) throws IOException {
        if (forceModifyReader) {
            StringBuilder newTemplate = new StringBuilder(formatter.getStartDocumentDirective());
            String oldTemplate = IOUtils.toString(reader);
            newTemplate.append(oldTemplate);
            newTemplate.append(formatter.getEndDocumentDirective());
            return new StringReader(newTemplate.toString());
        }
        return reader;
    }

    @Override
    public boolean isFieldNameStartsWithUpperCase() {
        throw new UnsupportedOperationException("Unimplemented method 'isFieldNameStartsWithUpperCase'");
    }
}
