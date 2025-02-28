package test;
import fr.opensagres.xdocreport.document.IXDocReport;
import fr.opensagres.xdocreport.document.docx.discovery.DocXReportFactoryDiscoveryTestCase;
import fr.opensagres.xdocreport.document.registry.XDocReportRegistry;
import fr.opensagres.xdocreport.template.ITemplateEngine;
import fr.opensagres.xdocreport.template.formatter.IDocumentFormatter;
import java.io.InputStream;

public class Main {
    public static void testPreprocess() throws Exception {
        InputStream is = DocXReportFactoryDiscoveryTestCase.class.getResourceAsStream("output_1739736866_all_rr.docx");
        try {
            IXDocReport report = XDocReportRegistry.getRegistry().loadReport(is, true);
            assertFalse("Report should not yet be preprocessed", report.isPreprocessed());
            assertNull("Original document archive should be null, since cacheOriginalDocument is false", report.getOriginalDocumentArchive());
            assertNotNull("Preprocessed document archive should be set", report.getPreprocessedDocumentArchive());
            report.setTemplateEngine(createTemplateEngine());
            report.preprocess();
            assertTrue("Report should be preprocessed now", report.isPreprocessed());
            assertNull("Original archive should still be null", report.getOriginalDocumentArchive());
            assertNotNull("Preprocessed document archive should still be set", report.getPreprocessedDocumentArchive());
        } finally {
            is.close();
        }
    }

    public static void main(String[] args) {
        testPreprocess();
    }
}

