#ifndef ANNOTATION_AGENT_H_
#define ANNOTATION_AGENT_H_

#include "onyx/data/annotation_document.h"

namespace anno
{

/// AnnotationAgent is agent to access all of the annotations in an application.
/// The annotation agent usually maintains a number of annotation documents. Each
/// document is composed by several annotation pages and each page contains a list
/// of concrete annotation objects.
/// Through AnnotationAgent, user can save and load all of the annotations in current
/// application. As an entry to access the annotation data, AnnotationAgent provides
/// method to retrieve given annotation document.
class AnnotationAgent
{
public:
    AnnotationAgent();
    ~AnnotationAgent();

    // Operate the document
    bool open( const QString & doc_path );
    bool close( const QString & doc_path );
    void close();
    bool save( const QString & doc_path );
    void save();
    bool loadPage( const QString & doc_path,
                   const PagePosition & page_position );
    bool loadAllPages( const QString & doc_path );

    AnnotationDocumentPtr getDocument( const QString & doc_path );

private:
    Documents docs_; // supports multiple documents,
                     // for virtual document
};

};

#endif
