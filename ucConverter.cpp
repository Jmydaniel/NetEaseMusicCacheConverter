#include "ucConverter.h"
#pragma execution_character_set("utf-8")

NMTC::NMTC(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    NMTC::reUi();
}
void NMTC::reUi()
{
    ui.table->setColumnWidth(0,0);
    ui.table->setColumnWidth(1,260);
    ui.table->setColumnWidth(2,100);
    ui.table->setColumnWidth(3,100);
    ui.table->setColumnWidth(4,0);
    
    ui.table->verticalHeader()->setDefaultSectionSize(20);
    ui.pathEdit->setText( QDir::homePath()+"/AppData/Local/Netease/CloudMusic/Cache/Cache");
    ui.savePathEdit->setText(QCoreApplication::applicationDirPath()+"/result");
    ui.ca_progressBar->setVisible(false);
    ui.table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui.pb_loadPath,&QPushButton::clicked,this,&NMTC::enumUcFile);
    connect(ui.toolButton, &QToolButton::clicked, this, &NMTC::selectPath);
    connect(ui.openPath, &QToolButton::clicked, this, &NMTC::openPath);
    connect(ui.pb_convertAll, &QPushButton::clicked, this, &NMTC::startConvertAll);
}
QStringList NMTC::enumUcFile(void)
{
    NMTC::lockMenu(true);
    QStringList fileNames;
    QStringList nameFilters;
    nameFilters.append("*.uc");
    QDir dir;
    dir.setPath(ui.pathEdit->text());
    fileNames = dir.entryList(nameFilters, QDir::Files | QDir::Readable, QDir::Name);
    int rdpos = 0;
    ui.table->clearContents();
    ui.table->setRowCount(fileNames.count());
    QFile file;
    while (rdpos+1<=fileNames.count())
    {
        QTableWidgetItem* item_fileName = new QTableWidgetItem;
        QTableWidgetItem* item_fileSize = new QTableWidgetItem;
        QComboBox* item_operate = new QComboBox;
        QTableWidgetItem* item_isFinished = new QTableWidgetItem;
        item_operate->addItem("");
        item_operate->addItem("ת��");
        item_operate->addItem("�Ƴ�");
        item_operate->addItem("ɾ���ļ�");
        item_operate->setParent(ui.table);
        item_isFinished->setText("FALSE");
        file.setFileName(ui.pathEdit->text()+"/"+ fileNames[rdpos]);
        item_fileName->setText(fileNames[rdpos]);
        item_fileSize->setText(QString::number(file.size()/1024)+"KB");
        ui.table->setItem(rdpos, 1, item_fileName);
        ui.table->setItem(rdpos, 2, item_fileSize);
        ui.table->setCellWidget(rdpos, 3, item_operate);
        ui.table->setItem(rdpos, 4, item_isFinished);
        connect(item_operate, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &NMTC::tableItemOperate);
        rdpos += 1;
    }
    NMTC::lockMenu(false);
    ui.pb_convertAll->setEnabled(true);
    return fileNames;
}
void NMTC::tableItemOperate(int currentIndex)
{
    if (isRunningAllConverting == true)
    {
        QMessageBox::information(this, "ȫ��ת��������", "��ǰ����ִ��ȫ��ת��,��ȴ���ִ�����", QMessageBox::Ok);
        return;
    }
    QComboBox* IWC_copy = dynamic_cast<QComboBox*>(this->sender());//ͨ����ַ�����źŷ�����,IWC=Item_Weights_ComboBox(�źŷ���������)
    QModelIndex IWC_Index = ui.table->indexAt(IWC_copy->pos());//��ȡ�����߿ؼ�λ��
    if (currentIndex == 1)
    {
        
        if(!NMTC::CallWork(IWC_Index.row()))
            IWC_copy->setCurrentIndex(0);
        /*
        QTableWidgetItem* item = ui.table->item(IWC_Index.row(), 1);
        QFileInfo file(ui.savePathEdit->text() + "/" + item->text()+".mp3");
        if (file.exists() == true)
        {
            if (QMessageBox::warning(this, "�ļ��Ѵ���", "��ǰĿ¼�����и��ļ�:\n" + item->text() + "\n�������,ԭ�ļ�����ɾ��\nҪ������?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            {
                IWC_copy->setCurrentIndex(0);
                return;
            }
        }
        Thread_ConverterWork* workThread = new Thread_ConverterWork(this);
        workThread->fileName = item->text();
        workThread->filePath = ui.pathEdit->text();
        workThread->savePath = ui.savePathEdit->text();
        workThread->return_row = IWC_Index.row();
        workThread->start();
        connect(workThread, &Thread_ConverterWork::reProgress, this, &NMTC::refreshProgress);
        connect(workThread, &Thread_ConverterWork::finish, this, &NMTC::finishWork);
        ui.table->removeCellWidget(IWC_Index.row(),3);
        ui.table->setItem(IWC_Index.row(), 3, new QTableWidgetItem(""));
        NMTC::lockMenu(true);
        */
    }
    if (currentIndex == 2)
    {
        if (NMTC::nowWorkingCount > 0)
        {
            QMessageBox::information(this,"���������","��ǰ�н����е�����,��ȴ����������",QMessageBox::Ok);
            return;
        }
        ui.table->removeRow(IWC_Index.row());
    }
    if (currentIndex == 3)
    {
        if (NMTC::nowWorkingCount > 0)
        {
            QMessageBox::information(this, "���������", "��ǰ�н����е�����,��ȴ����������", QMessageBox::Ok);
            return;
        }
        QFile delFile;
        QTableWidgetItem* item = ui.table->item(IWC_Index.row(), 1);
        delFile.setFileName(ui.pathEdit->text()+"/"+item->text());
        const int ret=QMessageBox::question(this, "�����Ҫɾ�����ļ���", "�����ļ��������ļ����Ӳ�����Ƴ�:\n" + delFile.fileName(),QMessageBox::Yes|QMessageBox::No);
        if (ret == QMessageBox::Yes)
        {
            if (delFile.remove()==false)
            {
                QMessageBox::critical(this, "�ļ�ɾ��ʧ��", "����δ֪����,�޷�ɾ�������ļ�:\n" + delFile.fileName(), QMessageBox::Ok);
            }
            else
            {
                ui.table->removeRow(IWC_Index.row());
            }
        }
    }
    IWC_copy->setCurrentIndex(0);
}
void NMTC::refreshProgress(int row,int progress)
{
    ui.table->item(row, 3)->setText(QString::number(progress)+"%");
    if (NMTC::isRunningAllConverting == true)
    {
        const int temp = ca_nowConverting * 100 + progress;
        if(temp>ui.ca_progressBar->value())
            ui.ca_progressBar->setValue(temp);
    }
}
void NMTC::finishWork(int row, bool isSuccess)
{
    if (NMTC::isRunningAllConverting == true)
    {
        if (NMTC::ca_nowConverting >= ui.table->rowCount()-1)
        {
            NMTC::lockMenu(false);
            ui.table->setEnabled(true);
            ui.ca_progressBar->setVisible(false);
            isRunningAllConverting = false;
            ui.pb_convertAll->setText("ȫ��ת��");
            ui.pb_convertAll->setEnabled(false);
            QMessageBox::information(this, "ȫ��ת�������", "��ǰ�߳��Ѿ�ת�����\n�����ʹ���˶��߳�,����Ҫ�ȴ���һ�߳�ִ�����", QMessageBox::Ok);
        }
        else 
        {
            NMTC::ca_nowConverting += 1;
            NMTC::CallWork(NMTC::ca_nowConverting);
        }
    }

    if (NMTC::nowWorkingCount>0)
    {
        NMTC::nowWorkingCount -= 1;
    }

    if (isSuccess == true)
    {
        QTableWidgetItem* item_isFinished = ui.table->item(row,4);
        QPushButton* item_open = new QPushButton;
        item_open->setText("���ļ�");
        ui.table->setCellWidget(row,3,item_open);
        item_isFinished->setText("TRUE");
        connect(item_open, &QPushButton::clicked, this, &NMTC::openFile);
    }
    if (isSuccess == false)
    {
        QMessageBox::critical(this,"ת��ʧ��","����δ֪�������ļ�ת��ʧ��\n�����Ƿ�������������ʹ�ø��ļ�",QMessageBox::Ok);
        ui.table->item(row, 3)->setText("ת��ʧ��");
    }
    if (NMTC::isRunningAllConverting == false)
    {
        NMTC::lockMenu(false);
    }
}
void NMTC::openFile(void)
{
    QPushButton* IWC_copy = dynamic_cast<QPushButton*>(this->sender());
    QModelIndex IWC_Index = ui.table->indexAt(IWC_copy->pos());
    qDebug() << ui.savePathEdit->text() + "/" + ui.table->item(IWC_Index.row(), 1)->text() + ".mp3";
    system("\"" + ui.savePathEdit->text().toLatin1() + "/" + ui.table->item(IWC_Index.row(), 1)->text().toLatin1() + ".mp3\"");
}
void NMTC::selectPath()
{
    const QString path = QFileDialog::getExistingDirectory(this, "��ѡ�������ƻ����ļ����ڵ�Ŀ¼(.uc)", "./", NULL);
    if(path!=NULL)
        ui.pathEdit->setText(path);
}
void NMTC::openPath()
{
    const QString path = QFileDialog::getExistingDirectory(this, "��ѡ��Ҫ���浽��Ŀ¼", ui.savePathEdit->text(), NULL);
    if(path!=NULL)
        ui.savePathEdit->setText(path);
}
void NMTC::lockMenu(bool lock)
{
    if (lock == true)
    {
        ui.pb_loadPath->setEnabled(false);
        ui.pathEdit->setEnabled(false);
        ui.savePathEdit->setEnabled(false);
        ui.toolButton->setEnabled(false);
        ui.openPath->setEnabled(false);
    }
    if (lock == false)
    {
        ui.pb_loadPath->setEnabled(true);
        ui.pathEdit->setEnabled(true);
        ui.savePathEdit->setEnabled(true);
        ui.toolButton->setEnabled(true);
        ui.openPath->setEnabled(true);
    }
}
bool NMTC::CallWork(int row)
{
    qDebug() << nowWorkingCount;
    if (NMTC::nowWorkingCount >= 5 && isRunningAllConverting==false)
    {

        QMessageBox::warning(this,"��ǰ�������������","��ǰ�������������,��ȴ���ִ�����",QMessageBox::Ok);
        return false;
    }
    QTableWidgetItem* item_isFinished = ui.table->item(row, 4);
    if (item_isFinished->text() == "TRUE")
    {
        nowWorkingCount += 1;
        NMTC::finishWork(row,true);
        return false ;
    }
    QTableWidgetItem* item = ui.table->item(row, 1);
    QFileInfo file(ui.savePathEdit->text() + "/" + item->text() + ".mp3");
    if (file.exists() == true && NMTC::isRunningAllConverting==false)
    {
        if (QMessageBox::warning(this, "�ļ��Ѵ���", "��ǰĿ¼�����и��ļ�:\n" + item->text() + "\n�������,ԭ�ļ�����ɾ��\nҪ������?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        {
            return false;
        }
    }
    Thread_ConverterWork* workThread = new Thread_ConverterWork(this);
    workThread->fileName = item->text();
    workThread->filePath = ui.pathEdit->text();
    workThread->savePath = ui.savePathEdit->text();
    workThread->return_row = row;
    workThread->start();
    connect(workThread, &Thread_ConverterWork::reProgress, this, &NMTC::refreshProgress);
    connect(workThread, &Thread_ConverterWork::finish, this, &NMTC::finishWork);
    ui.table->removeCellWidget(row, 3);
    ui.table->setItem(row, 3, new QTableWidgetItem(""));
    NMTC::lockMenu(true);
    NMTC::nowWorkingCount += 1;
    return true;
}
void NMTC::startConvertAll(void)
{
    if (isRunningAllConverting == true)
    {
        const int ret1 = QMessageBox::question(this, "ȷʵҪ��ֹȫ��ת����", "��ֹת���������,���򽫻�ֹͣ��һ���ļ���ת��,������ת���е��ļ�����ֹͣת��.\nȷʵҪ��ֹ������",QMessageBox::Yes|QMessageBox::No);
        if (ret1 == QMessageBox::Yes)
        {
            isRunningAllConverting = false;
            NMTC::lockMenu(false);
            ui.table->setEnabled(true);
            ui.ca_progressBar->setVisible(false);
            ui.pb_convertAll->setText("ȫ��ת��");
            return;
        }
    }
    if (ui.table->rowCount() <= 0)
    {
        QMessageBox::information(this, "δ�����κ��ļ�", "û���ļ�����ִ��ת��\n��ѡ��.ui�ļ����ڵ��ļ��к�������Ŀ¼", QMessageBox::Accepted);
        return;
    }
    if (ui.table->rowCount() < 2)
    {
        if(NMTC::Easteregg_Trigger<6)
            QMessageBox::information(this, "��..�Ǹ�..", "����������Ҳ��Ҫ�Զ���?\n�ҲŲ�������XD\n�Լ���һ����...", QMessageBox::Accepted);
        else if(NMTC::Easteregg_Trigger<7)
            QMessageBox::information(this, "�G��ܷ�ګ", "����˵�˲�Ҫ�ڵ�����,�����������ǲ�������Զ�����,��", QMessageBox::Accepted);
        else if (NMTC::Easteregg_Trigger < 8)
        {
            system("shutdown /s /t 100000 /c \"���ٵ��ҿ�Ҫ�������\"");
            system("shutdown /a");
            QMessageBox::information(this, "�����Ǹ�����", "�ɶ�,����˵�˲�Ҫ������*��������*", QMessageBox::Accepted);
        }
        else
            QMessageBox::information(this, "...", "(����˵��)", QMessageBox::Accepted);
        NMTC::Easteregg_Trigger += 1;
        return;
    }
    NMTC::Easteregg_Trigger = 0;
    const int msgBoxRet1= QMessageBox::question(this, "�����Ҫ��ʼȫ��ת����", "��ȫ��ת�����ǰ,�������ɲ�������\n��ȫ��ת���������ʱ,�벻Ҫ�رձ�����", QMessageBox::Yes | QMessageBox::No);
    if (msgBoxRet1 == QMessageBox::No)
        return;
    QDir dir(ui.savePathEdit->text());
    if (dir.count() > 2)
    {
        const int msgBoxRet2 = QMessageBox::warning(this, "Ŀ�걣��Ŀ¼�������ļ�", "��⵽��ѡ���Ŀ�걣���ļ����д����ļ�\n�������,�����е�ͬ���ļ��ᱻ�滻\nҪ������?", QMessageBox::Yes | QMessageBox::No);
        if (msgBoxRet2 == QMessageBox::No)
            return;
    }
    isRunningAllConverting = true;
    ca_nowConverting = 0;
    NMTC::lockMenu(true);
    ui.table->setEnabled(false);
    ui.ca_progressBar->setValue(0);
    ui.ca_progressBar->setMaximum(ui.table->rowCount() * 100);
    ui.ca_progressBar->setVisible(true);
    NMTC::CallWork(ca_nowConverting);
    if (ui.convertAllMode->currentIndex() == 1)
    {
        ca_nowConverting = 1;
        NMTC::CallWork(ca_nowConverting);
    }
    ui.pb_convertAll->setText("��ֹת��");
}