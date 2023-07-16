#include "mainwindow.h"
#include "ui_mainwindow.h"
#include<QSpinBox>
#include<QFontComboBox>
#include<QLabel>
#include<QProgressBar>
#include<QColorDialog>
#include<QFileDialog>
#include<QImageReader>
#include<QTextTable>
#include<QInputDialog>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //创建一些无法通过可视化UI直接设计的组件
    QSpinBox *spinFontSize=new QSpinBox(this);
    spinFontSize->setMinimum(5);//设置最小字体
    spinFontSize->setMaximum(50);//设置最大字体
    spinFontSize->setValue(ui->textEdit->font().pointSize());//设置默认字体
    spinFontSize->setMinimumWidth(60);//设置最小宽带
    ui->toolBar->addWidget(spinFontSize);

    QFontComboBox *comboFontBox=new QFontComboBox;
    comboFontBox->setMinimumWidth(150);
    comboFontBox->setFont(ui->textEdit->font());
    ui->toolBar->addWidget(comboFontBox);

    labelFile=new QLabel(this);
    labelFile->setMinimumWidth(150);
    labelFile->setText("文件名：");
    ui->statusbar->addWidget(labelFile);
    progressBar=new QProgressBar(this);
    progressBar->setMinimum(5);
    progressBar->setMaximum(50);
    progressBar->setValue(ui->textEdit->font().pointSize());
    ui->statusbar->addWidget(progressBar);
    labInfo=new QLabel("Permanent");
    ui->statusbar->addPermanentWidget(labInfo);

    connect(spinFontSize,&QSpinBox::valueChanged,this,&MainWindow::on_fontSize_changed);
    connect(comboFontBox,&QFontComboBox::currentFontChanged,this,&MainWindow::on_font_changed);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actFile_Open_triggered()
{
    QString curPath=QDir::currentPath();
    QString filter="文本文件(*.txt)";
    QString fileName=QFileDialog::getOpenFileName(this,"打开文本文件",curPath,filter);
    if(fileName.isEmpty()){
        return ;
    }
    QFile file(fileName);
    if(file.open(QIODevice::ReadWrite)){
        ui->textEdit->setText(file.readAll());
        file.close();
    }
    labelFile->setText("文件名："+fileName);
}


void MainWindow::on_actFile_Save_triggered()
{
    QString curPath = QDir::currentPath();
    QString filter = "文本文件(*.txt)";
    QString fileName = QFileDialog::getSaveFileName(this,"另存为",curPath,filter);
    if(fileName.isEmpty()){
         return;
    }
    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly)){
        QString content = ui->textEdit->toPlainText();
        QByteArray strBytes = content.toUtf8();
        file.write(strBytes,strBytes.length());
    }
     ui->textEdit->document()->setModified(true);
     labelFile->setText("文件已保存");
}


void MainWindow::on_textEdit_copyAvailable(bool b)
{
     ui->actEdit_Copy->setEnabled(b);
     ui->actEdit_Cut->setEnabled(b);
     ui->actEdit_Paste->setEnabled((ui->textEdit->canPaste()));
}


void MainWindow::on_textEdit_selectionChanged()
{
     QTextCharFormat fmt=ui->textEdit->currentCharFormat();
     ui->actFont_Bold->setChecked(fmt.font().bold());
     ui->actFont_Italic->setChecked(fmt.font().italic());
     ui->actFont_UnderLine->setChecked(fmt.font().underline());
}


void MainWindow::on_actFont_Bold_triggered(bool checked)
{
     QTextCharFormat fmt=ui->textEdit->currentCharFormat();
     if(checked){
          fmt.setFontWeight(QFont::Bold);
     }else{
          fmt.setFontWeight(QFont::Normal);
     }
     ui->textEdit->setCurrentCharFormat(fmt);
}


void MainWindow::on_actFont_Italic_toggled(bool arg1)
{
     QTextCharFormat fmt=ui->textEdit->currentCharFormat();
     fmt.setFontItalic(arg1);
     ui->textEdit->setCurrentCharFormat(fmt);
}


void MainWindow::on_actFont_UnderLine_triggered(bool checked)
{
    QTextCharFormat fmt=ui->textEdit->currentCharFormat();
    fmt.setFontUnderline(checked);
    ui->textEdit->setCurrentCharFormat(fmt);
}

void MainWindow::on_fontSize_changed(int fontSize)
{
    QTextCharFormat fmt;
    fmt.setFontPointSize(fontSize);
    ui->textEdit->mergeCurrentCharFormat(fmt);
    progressBar->setValue(fontSize);
}

void MainWindow::on_font_changed(const QFont &font)
{
    QTextCharFormat fmt;
    int infont=ui->textEdit->fontPointSize();
    fmt.setFont(font);
    fmt.setFontPointSize(infont);
    ui->textEdit->mergeCurrentCharFormat(fmt);
    labInfo->setText("字体名称: "+font.family());
}

void MainWindow::on_actFont_Color_triggered(bool checked)
{
    QPalette pal=ui->textEdit->palette();
    QColor iniColor=pal.color(QPalette::Text);
    QColor color=QColorDialog::getColor(iniColor,this,"选择颜色");
    if(!color.isValid())
    {
          return ;
    }
    QTextCharFormat fmt=ui->textEdit->currentCharFormat();
    fmt.setForeground(color);
    ui->textEdit->setCurrentCharFormat(fmt);
}

void MainWindow::on_actEdit_Picture_triggered()
{
    QFileDialog fileDialog(this,tr("open File......"));
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.setNameFilter(tr("Image(*.bmp *.jpg *jpe *.gif *.png)"));//筛选图片格式
    if(fileDialog.exec()!=QDialog::Accepted)
    {
          return ;
    }
    const QString fn=fileDialog.selectedFiles().first();
    QUrl Uri(QString("file://%1").arg(fn));
    QImage image = QImageReader(fn).read();
    QTextDocument * textDocument = ui->textEdit->document();
    textDocument->addResource(QTextDocument::ImageResource, Uri, QVariant(image));
    QTextCursor cursor = ui->textEdit->textCursor();
    QTextImageFormat imageFormat;
    imageFormat.setWidth(image.width());
    imageFormat.setHeight(image.height());
    imageFormat.setName(Uri.toString());
    cursor.insertImage(imageFormat);
}


void MainWindow::on_actEdit_Table_triggered()
{
    int rows, cols;
    double padding;
    bool ok;
    rows = QInputDialog::getInt(this, QString("获取行数"), QString("行:"), 1, 1, 10000, 1, &ok);
    if (!ok){
        return;
    }
    cols = QInputDialog::getInt(this, QString("获取列数"), QString("列:"), 1, 1, 10000, 1, &ok);
    if (!ok){
        return;
    }
    padding = QInputDialog::getDouble(this, QString("设置单元格大小"), QString("像素:"), 1, 1, 100, 1, &ok);
    if (!ok){
        return;
    }
    QTextTableFormat fmt;
    fmt.setCellPadding(padding);
    fmt.setAlignment(ui->textEdit->alignment());
    QTextTable * table = ui->textEdit->textCursor().insertTable(rows, cols, fmt);
}


void MainWindow::on_actEdit_Left_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
    ui->actEdit_Left->setChecked(true);
    ui->actEdit_Right->setChecked(false);
    ui->actEdit_Center->setChecked(false);
    ui->actEdit_Side->setChecked(false);
}


void MainWindow::on_actEdit_Right_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
    ui->actEdit_Left->setChecked(false);
    ui->actEdit_Right->setChecked(true);
    ui->actEdit_Center->setChecked(false);
    ui->actEdit_Side->setChecked(false);
}


void MainWindow::on_actEdit_Center_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignHCenter);
    ui->actEdit_Left->setChecked(false);
    ui->actEdit_Right->setChecked(false);
    ui->actEdit_Center->setChecked(true);
    ui->actEdit_Side->setChecked(false);
}


void MainWindow::on_actEdit_Side_triggered()
{
    ui->textEdit->setAlignment(Qt::AlignJustify);
    ui->actEdit_Left->setChecked(false);
    ui->actEdit_Center->setChecked(false);
    ui->actEdit_Right->setChecked(false);
    ui->actEdit_Side->setChecked(true);
}

