#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QList>
#include <QMessageBox>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMimeData>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(QSize(500,570));
    this->initUI();
}

void MainWindow::initUI(){
    setAcceptDrops(true);
    QList<QString> kinds;
    kinds<<"Application"<<"Link";
    foreach(QString kind,kinds ){
        ui->kinds->addItem(kind);
    }
    QList<QString> categorys;
    categorys<<"AudioVideo"<<"Audio"<<"Video"<<"Development"<<"Education"<<"Game"<<"Graphics"<<"Network"<<"Office"<<"Settings"<<"System"<<"Utility";
    foreach(QString category,categorys ){
        ui->category->addItem(category);
    }
    connect(ui->kinds,SIGNAL(currentIndexChanged(int)),this,SLOT(kindChanged(int)));
    connect(ui->iconFileButton,&QPushButton::clicked,this,&MainWindow::chooseIconFile);
    connect(ui->execButton,&QPushButton::clicked,this,&MainWindow::chooseAppFile);
    connect(ui->createButton,&QPushButton::clicked,this,&MainWindow::onCreate);
    ui->name->setAcceptDrops(false);
    ui->genericName->setAcceptDrops(false);
    ui->comment->setAcceptDrops(false);
    ui->iconFile->setAcceptDrops(false);
    ui->exec->setAcceptDrops(false);

}
MainWindow::~MainWindow()
{

    delete ui;
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event){

    if(event->mimeData()->hasFormat("text/uri-list")){
        event->acceptProposedAction();
    }else{
        event->ignore();
    }
}
void MainWindow::dropEvent(QDropEvent* event){

    auto urls=event->mimeData()->urls();

    if(urls.isEmpty())
        return ;

    QString fileName = urls.first().toLocalFile();
    handleDropFile(fileName);

}
void MainWindow::chooseIconFile(){
    auto path=QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString iconFile=QFileDialog::getOpenFileName(this,tr("tr_image_caption"),path,tr("Images (*.png *.xpm *.jpg)"));
    if(!iconFile.isEmpty()){
        ui->iconFile->setText(iconFile);
    }
}
void MainWindow::chooseAppFile(){
    auto path=QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    QString execFile=QFileDialog::getOpenFileName(this,tr("tr_image_caption"),path);
    if(!execFile.isEmpty()){
        ui->exec->setText(execFile);
    }
}
void MainWindow::kindChanged(int index){
    if(index==0){//application
        ui->execLabel->setText(tr("exec"));
        ui->terminal->setEnabled(true);
        ui->notify->setEnabled(true);
        ui->category->setEnabled(true);
    }else{
        ui->execLabel->setText(tr("link"));
        ui->terminal->setCheckState(Qt::Unchecked);
        ui->terminal->setEnabled(false);
        ui->notify->setCheckState(Qt::Unchecked);
        ui->notify->setEnabled(false);
        ui->category->setEnabled(false);
    }
}
void MainWindow::onCreate(){
    auto name=ui->name->text().trimmed();
    if(name.isNull()||name.isEmpty()){
        QMessageBox::information(this,tr("message_title"),tr("name_is_missing"),QMessageBox::Ok);
        return;
    }
    auto genericName=ui->genericName->text().trimmed();
    auto comment=ui->comment->text().trimmed();
    auto icon=ui->iconFile->text().trimmed();
    auto exec=ui->exec->text().trimmed();
    auto terminal=ui->terminal->isChecked();
    auto notify=ui->notify->isChecked();
    if(exec.isEmpty()){
        QMessageBox::information(this,tr("message_title"),tr("exec_is_missing"),QMessageBox::Ok);
        return;
    }
    QMap<QString,QString> contents;
    contents["Name"]=name;
    contents["Version"]="1.0";
    if(!genericName.isEmpty()){
        contents["GenericName"]=genericName;
    }
    if(!comment.isEmpty()){
        contents["Comment"]=comment;
    }
    if(this->isImageFile(icon)){
        contents["Icon"]=icon;
    }
    int index=ui->kinds->currentIndex();
    auto kind=ui->kinds->currentText();

    contents["Type"]=kind;
    if(index==0){
        auto category=ui->category->currentText();
        contents["Categories"]=category;
        if(terminal){
            contents["Terminal"]="true";
        }else{
            contents["Terminal"]="false";
        }

        if(notify){
            contents["StartupNotify"]="true";
        }else{
            contents["StartupNotify"]="false";
        }
        contents["Exec"]=exec;

    }else{
        contents["URL[$e]"]=exec;
    }
    auto realFileName=QString("%1.desktop").arg(name);
    auto dir=QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    auto path=QFileDialog::getExistingDirectory(this,tr("file_dialog_save_caption"),dir,QFileDialog::ShowDirsOnly|QFileDialog::DontResolveSymlinks);
    if(!path.isEmpty()){
        auto realPath=QString("%1/%2").arg(path,realFileName);
        auto ok=this->writeEntryFile(contents,realPath);
        if(ok){
            QMessageBox::information(this,tr("message_title"),tr("entry_write_success"));
        }else{
            QMessageBox::information(this,tr("message_title"),tr("entry_write_fail"));
        }
    }
}

bool MainWindow::isImageFile(const QString& file){
    if(!file.isEmpty()){
        QFileInfo info(file);
        if(info.exists()){
            QMimeDatabase database;
            auto mimeType=database.mimeTypeForFile(file);
            if(mimeType.isValid()){
                auto name=mimeType.name().toLower();
                return name.startsWith("image");
            }
        }
    }
    return false;
}

bool MainWindow::writeEntryFile(const QMap<QString,QString>& entries,const QString& realFileName){
    if(entries.isEmpty()){
        return false;
    }
    QFile file(realFileName);
    bool ok=file.open(QIODevice::ReadWrite|QIODevice::Truncate|QIODevice::Text);
    if(ok){
        bool ret=true;
        auto len=file.write("[Desktop Entry]\n");
        if(len>0){
            for(auto iter=entries.begin();iter!=entries.end();++iter){
                auto content=QString("%1=%2\n").arg(iter.key(),iter.value());
                len=file.write(content.toUtf8());
                if(len<=0){
                    ret=false;
                    break;
                }
            }
        }else{
            ret=false;
        }
        file.close();
        return ret;
    }

    return false;
}

void MainWindow::handleDropFile(const QString& filename){
    if(isImageFile(filename)){
        ui->iconFile->setText(filename);
    }else{
        QFileInfo file(filename);
        ui->name->clear();
        ui->name->setText(file.baseName());
        ui->exec->clear();
        ui->exec->setText(filename);
        ui->genericName->clear();
        ui->genericName->setText(file.fileName());
        ui->comment->clear();
        ui->kinds->setCurrentIndex(0);
        ui->terminal->setCheckState(Qt::Unchecked);
        ui->notify->setCheckState(Qt::Unchecked);
    }

}

