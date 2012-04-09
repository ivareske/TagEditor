/****************************************************************************
**
** Copyright (c) 2011 Digia Plc
** All rights reserved.
** For any questions to Digia, please use contact form at http://qt.digia.com
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia nor the names of its contributors
**     may be used to endorse or promote products derived from this 
**     software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
** 
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
**
****************************************************************************/

#include <QtGui>

#include "codeeditor.h"


CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent){
    lineNumberArea = new LineNumberArea(this);

    s = Global::guiSettings(this);
    s->beginGroup("CodeEditor");
    QFont font = s->value("font",this->font()).value<QFont>();
    this->setFont(font);
    s->endGroup();

    beautifyAction = new QAction("Beautify javascript code",this);
    connect(beautifyAction, SIGNAL(triggered()), this, SLOT(beautify()));
    beautifyAction->setShortcut(tr("Ctrl+B"));
    addAction(beautifyAction);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();
}

CodeEditor::~CodeEditor() {
    delete s;s=0;
}

void CodeEditor::beautify(){

    //QString text = this->textCursor().selectedText();
    QString text = this->toPlainText();
    int current = currentLine();    
    text = beautifyJavaScriptCode(text);
    this->setPlainText(text);
    this->setCurrentLine(current);    

}

int CodeEditor::currentLine() const{
    return this->textCursor().blockNumber();
}

void CodeEditor::setCurrentLine(int line){
    QTextCursor c = this->textCursor();
    c.movePosition(QTextCursor::Down,QTextCursor::MoveAnchor, line);
    this->setTextCursor(c);
}

QString CodeEditor::beautifyJavaScriptCode( const QString &source ){


    QScriptEngine engine;
    QScriptValue options = engine.newObject();

    options.setProperty("preserve_newlines", true);
    options.setProperty("space_after_anon_function", true);
    options.setProperty("braces_on_own_line", false);
    options.setProperty("keep_array_indentation", true);


    if (source.isEmpty()) {
        return source;
    }

    QString fileName = qApp->applicationDirPath()+"/beautify.js";
    QFile file(fileName);
    if(!file.exists()){
        QMessageBox::critical(0,"","Could not find 'beautify.js' in application directory! Cannot beautify code.");
        return source;
    }


    if(!file.open(QFile::ReadOnly)) {
        return source;
    }
    QString script = file.readAll();
    file.close();

    if (script.isEmpty()) {
        return source;
    }

    engine.evaluate(script);
    engine.globalObject().setProperty("source", QScriptValue(source));
    engine.globalObject().setProperty("options", options);
    QScriptValue result = engine.evaluate("js_beautify(source, options);");

    return result.toString();
}


void CodeEditor::contextMenuEvent(QContextMenuEvent *event){
    QMenu *menu = this->createStandardContextMenu();
    menu->addAction(beautifyAction);

    QAction *editFontAction = new QAction("Edit font...",this);
    connect(editFontAction,SIGNAL(triggered()),this,SLOT(fontDialog()));
    menu->addAction(editFontAction);

    if(!example_.isEmpty()){
        QAction *insertExampleAction = new QAction("Insert example code",this);
        connect(insertExampleAction,SIGNAL(triggered()),this,SLOT(insertExample()));
        menu->addAction(insertExampleAction);
    }
    menu->exec(event->globalPos());
}


int CodeEditor::lineNumberAreaWidth(){
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}



void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */){ 
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


void CodeEditor::updateLineNumberArea(const QRect &rect, int dy){
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::focusOutEvent(QFocusEvent *e) {

    emit editingFinished();
    QPlainTextEdit::focusOutEvent(e);
}

void CodeEditor::resizeEvent(QResizeEvent *e){
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}



void CodeEditor::highlightCurrentLine(){
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;
        
        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}


void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event){
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::setExample(const QString &example){
    example_=example;
    if(this->toolTip().isEmpty()){
        this->setToolTip(example);
    }
}

QString CodeEditor::example() const{
    return example_;
}

void CodeEditor::insertExample(){
    this->insertPlainText(example_);
}

void CodeEditor::setText(const QString &text){
    this->setPlainText(text);
}

QString CodeEditor::text() const{
    return this->toPlainText();
}

/*!
 \brief Show a dialog to edit the font of the texteditor

*/
void CodeEditor::fontDialog(){

    bool ok;
    QFont font = QFontDialog::getFont(&ok, this->font(), this);

    if( ok ){
        this->setFont(font);
        s->beginGroup("CodeEditor");
        s->setValue("font",this->font());
        s->endGroup();
        s->sync();
    }
}
