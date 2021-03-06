#include "drugstatistics.h"
#include <QtGui>
#include "connectsql.h"
#include "clinicdrugfilter.h"
#include "package.h"
#include "drugstorage.h"
#include "drugallot.h"
#include "drugcheck.h"
#include "drugpricing.h"
#include "odbcexcel.h"
static int icount;
extern ConnectSql sql;

DrugStatistics::DrugStatistics(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initUI();
	druglist = new QListView(this);
	model = new QStringListModel(this);
	druglist->setWindowFlags(Qt::ToolTip);
	connect(druglist, SIGNAL(clicked(const QModelIndex &)), this, SLOT(completeText(const QModelIndex &)));
	setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: grey;}");
	ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	ui.tableWidget->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: grey;}");
	ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	
	ui.tableWidget->installEventFilter(this);//注册事件过滤器
	connect(ui.lineEdit_DrugName,SIGNAL(textChanged(const QString &)),this,SLOT(DrugName(const QString &)));
	connect(ui.tableWidget,SIGNAL(cellDoubleClicked(int,int)),this,SLOT(getItem(int,int)));
}

DrugStatistics::~DrugStatistics()
{

}

void DrugStatistics::DrugName(const QString &text)
{
	druglist->hide();
	if (text.isEmpty()) {
		druglist->hide();
		return;
	}

	if ((text.length() > 1) && (!druglist->isHidden())) {
		return;
	}

	QSqlQuery query(*sql.db);	

	QString strsql=QString("select * from sys_drugdictionary where abbr like '%%1%' or name like'%%2%'  ").arg(text).arg(text);
	query.exec(strsql);
	QStringList list;
	while(query.next())
	{
		QString str = query.value(1).toString();
		list.append(str);
	}

	model->setStringList(list);
	//	model->setStringList(sl);
	druglist->setModel(model);

	if (model->rowCount() == 0) {
		return;
	}

	// Position the text edit
	druglist->setMinimumWidth(width());
	druglist->setMaximumWidth(width());

	//druglist->setGeometry(this->x()+822, this->y()+165, 50, 100);
	QPoint GlobalPoint(ui.lineEdit_DrugName->mapToGlobal(QPoint(0, 0)));
	druglist->setGeometry(GlobalPoint.x(), GlobalPoint.y()+ui.lineEdit_DrugName->height(), 60, 100);
	druglist->show();

}

void DrugStatistics::on_radioButton_clicked()
{
	if(!ui.radioButton->isChecked())
	{
		//ui.lineEdit_DrugNo->setEnabled(false);
		ui.lineEdit_DrugName->setEnabled(false);
	}
	if(ui.radioButton->isChecked())
	{
		//ui.lineEdit_DrugNo->setEnabled(true);
		ui.lineEdit_DrugName->setEnabled(true);
	}
		
}

void DrugStatistics::initUI()
{
	
	iRow=0;
	QSqlQuery query(*sql.db);		
	QDateTime current_date_time = QDateTime::currentDateTime();
	//ui.dateTimeEdit_1->setDateTime(current_date_time);
	//ui.dateTimeEdit_2->setDateTime(current_date_time);
	//ui.saveButton->setEnabled(true);
	//ui.discardButton->setEnabled(true);
	ui.dateTimeEdit_1->setDateTime(current_date_time);
	QTime time= QTime::fromString("00:00:00", "hh:mm:ss");
	ui.dateTimeEdit_1->setTime(time);
	ui.dateTimeEdit_2->setDateTime(current_date_time);
	time= QTime::fromString("23:59:59", "hh:mm:ss");
	ui.dateTimeEdit_2->setTime(time);

	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //整行选中的方式
	QTextCodec::setCodecForTr(QTextCodec::codecForName("gb18030"));
	if(!ui.radioButton->isChecked())
	{
		//ui.lineEdit_DrugNo->setEnabled(false);
		ui.lineEdit_DrugName->setEnabled(false);
	}
	if(ui.radioButton->isChecked())
	{
		//ui.lineEdit_DrugNo->setEnabled(true);
		ui.lineEdit_DrugName->setEnabled(true);
	}
}
void DrugStatistics::getItem(int row,int column)
{
	QString strFindItem;
	strFindItem = ui.comboBox_find->currentText();
	std::string strStd= strFindItem.toStdString();
	QTextCodec *code = QTextCodec::codecForName("gb18030");
	 // 如果code为0，表示在运行的机器上没有装gb18030字符集。不过一般的中文windows系统中都安装了这一字符集
	if (code)   
		strStd= code->fromUnicode(strFindItem).data();

	
	if(strStd == "入库记录"||strStd == "退货记录")
	{
		DrugStorage *storage = new DrugStorage();
		QString strNo= ui.tableWidget->item(row,0)->text();
		storage->resize(943,575);
		storage->show();
		storage->edit(strNo);
	}
	if(strStd == "销售调拨记录")
	{
		DrugAllot *allot = new DrugAllot();
		QString strNo= ui.tableWidget->item(row,0)->text();
		allot->resize(943,575);
		allot->show();
		allot->edit_sale(strNo);
	}
	if(strStd == "仓库调拨记录")
	{
		DrugAllot *allot = new DrugAllot();
		QString strNo= ui.tableWidget->item(row,0)->text();
		allot->resize(943,575);
		allot->show();
		allot->edit_store(strNo);
	}
	if(strStd == "盈亏损益")
	{
		DrugCheck *check = new DrugCheck();
		QString strNo= ui.tableWidget->item(row,0)->text();
		check->resize(943,575);
		check->show();
		check->edit(strNo);
	}
	if(strStd == "调价记录")
	{
		DrugPricing *price = new DrugPricing();
		QString strNo= ui.tableWidget->item(row,0)->text();
		price->resize(943,575);
		price->show();
		price->edit(strNo);
	}
	//clinicchargedlg chargedlg = new clinicchargedlg;
	//chargedlg.setWindowModality(Qt::ApplicationModal); //阻塞除当前窗体之外的所有的窗体
	//chargedlg.exec();
	//QString strNo= ui.tableWidget->item(row,10)->text();
	//chargedlg->edit(strNo);

	/*ConnectDB connectdatase;
	connectdatase.exec();*/
}
void DrugStatistics::on_findButton_clicked()
{
	QString strFindItem;
	QString strDate1;
	QString strDate2;
	QString strsql;
	QString strName;
	strFindItem = ui.comboBox_find->currentText();
	QDateTime time;	
	time = ui.dateTimeEdit_1->dateTime();
	strDate1 = time.toString("yyyy-MM-dd hh:mm:ss");

	time = ui.dateTimeEdit_2->dateTime();
	strDate2 = time.toString("yyyy-MM-dd hh:mm:ss");


	int rows = ui.tableWidget->model()->rowCount();   //行总数
	for (int i=0;i<rows+1;i++)
	{
		ui.tableWidget->removeRow(rows - i);
	}



	QSqlQuery query(*sql.db);

	std::string strStd= strFindItem.toStdString();
	QTextCodec *code = QTextCodec::codecForName("gb18030");
	 // 如果code为0，表示在运行的机器上没有装gb18030字符集。不过一般的中文windows系统中都安装了这一字符集
	if (code)   
		strStd= code->fromUnicode(strFindItem).data();

	rows = ui.tableWidget->model()->rowCount();   //行总数
		for (int i=0;i<rows+1;i++)
		{
			ui.tableWidget->removeRow(rows - i);
		}

	if(strStd == "入库记录"||strStd == "退货记录")
	{
		ui.tableWidget->setColumnCount(9); //设置列数
		ui.tableWidget->horizontalHeader()->setClickable(false); //设置表头不可点击（默认点击后进行排序）

		QStringList header;
		if(strStd == "入库记录")
			header<<tr("单号")<<tr("日期")<<tr("名称")<<tr("规格")<<tr("生产厂商")<<tr("单位")<<tr("入库数量")<<tr("入库价格")<<tr("入库总额");
		else
			header<<tr("单号")<<tr("日期")<<tr("名称")<<tr("规格")<<tr("生产厂商")<<tr("单位")<<tr("退货数量")<<tr("退货价格")<<tr("退货总额");
		ui.tableWidget->setHorizontalHeaderLabels(header);

		ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		ui.tableWidget->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: white;}");
		ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	

		if(ui.radioButton->isChecked())
		{
			int itotalNum=0;
			double dtotalPrice=0;
			strName = ui.lineEdit_DrugName->text();
			strsql= "select * from yk_instorage where name='"+strName+"'and date between '"+strDate1+"' and '"+strDate2+"'";
			query.exec(strsql);
			iRow = 0;
			while(query.next())
			{
				int num;			
				num= query.value(13).toInt();
				if(strStd == "入库记录"&& num<0) continue;
				if(strStd == "退货记录"&& num>0) continue;
				if(strStd == "退货记录"&& num<0) num= query.value(13).toInt()*(-1);
				int rows = ui.tableWidget->model()->rowCount();   //行总数
				ui.tableWidget->insertRow(rows);
				iRow=rows;
				ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(1).toString()));
				ui.tableWidget->setItem(iRow,1,new QTableWidgetItem(query.value(2).toString()));
				ui.tableWidget->setItem(iRow,2,new QTableWidgetItem(query.value(8).toString()));
				ui.tableWidget->setItem(iRow,3,new QTableWidgetItem(query.value(9).toString()));
				ui.tableWidget->setItem(iRow,4,new QTableWidgetItem(query.value(10).toString()));
				ui.tableWidget->setItem(iRow,5,new QTableWidgetItem(query.value(12).toString()));//
				ui.tableWidget->setItem(iRow,6,new QTableWidgetItem(QString::number(num)));//
				ui.tableWidget->setItem(iRow,7,new QTableWidgetItem(query.value(14).toString()));//
				ui.tableWidget->setItem(iRow,8,new QTableWidgetItem(query.value(15).toString()));//
				
				//int num;
				//if(strStd == "入库记录")
				//	num= query.value(13).toInt();
				//else
				//	num= query.value(13).toInt()*(-1);
				itotalNum = itotalNum + num;
				double price = query.value(15).toDouble();
				dtotalPrice = dtotalPrice + price;
				//iRow ++;
			}
			QString strProfit = QString::number(itotalNum);
			ui.lineEdit_TotalNum->setText(strProfit);
			strProfit = QString::number(dtotalPrice);
			ui.lineEdit_TotalPrice->setText(strProfit);
		}
		if(!ui.radioButton->isChecked())
		{
			int itotalNum =0;
			double dtotalPrice=0;
			strName = ui.lineEdit_DrugName->text();
			strsql= "select * from yk_instorage where date between '"+strDate1+"' and '"+strDate2+"'";
			query.exec(strsql);
			iRow = 0;
			while(query.next())
			{
				int num;			
				num= query.value(13).toInt();
				if(strStd == "入库记录"&& num<0) continue;
				if(strStd == "退货记录"&& num>0) continue;
				if(strStd == "退货记录"&& num<0) num= query.value(13).toInt()*(-1);

				int rows = ui.tableWidget->model()->rowCount();   //行总数
				ui.tableWidget->insertRow(rows);
				iRow=rows;
				ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(1).toString()));
				ui.tableWidget->setItem(iRow,1,new QTableWidgetItem(query.value(2).toString()));
				ui.tableWidget->setItem(iRow,2,new QTableWidgetItem(query.value(8).toString()));
				ui.tableWidget->setItem(iRow,3,new QTableWidgetItem(query.value(9).toString()));
				ui.tableWidget->setItem(iRow,4,new QTableWidgetItem(query.value(10).toString()));
				ui.tableWidget->setItem(iRow,5,new QTableWidgetItem(query.value(12).toString()));//
				ui.tableWidget->setItem(iRow,6,new QTableWidgetItem(QString::number(num)));//
				ui.tableWidget->setItem(iRow,7,new QTableWidgetItem(query.value(14).toString()));//
				ui.tableWidget->setItem(iRow,8,new QTableWidgetItem(query.value(15).toString()));//

				itotalNum = itotalNum + num;
				double price = query.value(15).toDouble();
				dtotalPrice = dtotalPrice + price;
			}
			QString strProfit = QString::number(itotalNum);
			ui.lineEdit_TotalNum->setText(strProfit);
			strProfit = QString::number(dtotalPrice);
			ui.lineEdit_TotalPrice->setText(strProfit);
		}

	}
	else if(strStd == "销售调拨记录")
	{
		ui.tableWidget->setColumnCount(10); //设置列数
		ui.tableWidget->horizontalHeader()->setClickable(false); //设置表头不可点击（默认点击后进行排序）

		QStringList header;
		header<<tr("单号")<<tr("日期")<<tr("客户")<<tr("名称")<<tr("规格")<<tr("生产厂商")<<tr("单位")<<tr("调拨数量")<<tr("价格")<<tr("总价");
		ui.tableWidget->setHorizontalHeaderLabels(header);

		ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		ui.tableWidget->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: white;}");
		ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	

		if(ui.radioButton->isChecked())
		{
			int itotalNum=0;
			double dtotalPrice=0;
			strName = ui.lineEdit_DrugName->text();
			strsql= "select * from yk_saleallot where name='"+strName+"'and date between '"+strDate1+"' and '"+strDate2+"'";
			query.exec(strsql);
			iRow = 0;
			while(query.next())
			{
				int rows = ui.tableWidget->model()->rowCount();   //行总数
				ui.tableWidget->insertRow(rows);
				iRow=rows;
				ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(1).toString()));
				ui.tableWidget->setItem(iRow,1,new QTableWidgetItem(query.value(2).toString()));
				ui.tableWidget->setItem(iRow,2,new QTableWidgetItem(query.value(5).toString()));
				ui.tableWidget->setItem(iRow,3,new QTableWidgetItem(query.value(8).toString()));
				ui.tableWidget->setItem(iRow,4,new QTableWidgetItem(query.value(9).toString()));
				ui.tableWidget->setItem(iRow,5,new QTableWidgetItem(query.value(10).toString()));//单位
				ui.tableWidget->setItem(iRow,6,new QTableWidgetItem(query.value(12).toString()));//
				ui.tableWidget->setItem(iRow,7,new QTableWidgetItem(query.value(13).toString()));//
				ui.tableWidget->setItem(iRow,8,new QTableWidgetItem(query.value(14).toString()));//
				ui.tableWidget->setItem(iRow,9,new QTableWidgetItem(query.value(15).toString()));//
				int num = query.value(13).toInt();
				itotalNum = itotalNum + num;
				double price = query.value(15).toDouble();
				dtotalPrice = dtotalPrice + price;
			}
			QString strProfit = QString::number(itotalNum);
			ui.lineEdit_TotalNum->setText(strProfit);
			strProfit = QString::number(dtotalPrice);
			ui.lineEdit_TotalPrice->setText(strProfit);
		}
		if(!ui.radioButton->isChecked())
		{
			int itotalNum=0;
			double dtotalPrice=0;
			strName = ui.lineEdit_DrugName->text();
			strsql= "select * from yk_saleallot where date between '"+strDate1+"' and '"+strDate2+"'";
			query.exec(strsql);
			iRow = 0;
			while(query.next())
			{
				int rows = ui.tableWidget->model()->rowCount();   //行总数
				ui.tableWidget->insertRow(rows);
				iRow=rows;
				ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(1).toString()));
				ui.tableWidget->setItem(iRow,1,new QTableWidgetItem(query.value(2).toString()));
				ui.tableWidget->setItem(iRow,2,new QTableWidgetItem(query.value(5).toString()));
				ui.tableWidget->setItem(iRow,3,new QTableWidgetItem(query.value(8).toString()));
				ui.tableWidget->setItem(iRow,4,new QTableWidgetItem(query.value(9).toString()));
				ui.tableWidget->setItem(iRow,5,new QTableWidgetItem(query.value(10).toString()));
				ui.tableWidget->setItem(iRow,6,new QTableWidgetItem(query.value(12).toString()));//
				ui.tableWidget->setItem(iRow,7,new QTableWidgetItem(query.value(13).toString()));//
				ui.tableWidget->setItem(iRow,8,new QTableWidgetItem(query.value(14).toString()));//
				ui.tableWidget->setItem(iRow,9,new QTableWidgetItem(query.value(15).toString()));//
				//iRow ++;
				int num = query.value(13).toInt();
				itotalNum = itotalNum + num;
				double price = query.value(15).toDouble();
				dtotalPrice = dtotalPrice + price;
			}
			QString strProfit = QString::number(itotalNum);
			ui.lineEdit_TotalNum->setText(strProfit);
			strProfit = QString::number(dtotalPrice);
			ui.lineEdit_TotalPrice->setText(strProfit);
		}
	}
	else if(strStd == "仓库调拨记录")
	{
		
		ui.tableWidget->setColumnCount(9); //设置列数
		ui.tableWidget->horizontalHeader()->setClickable(false); //设置表头不可点击（默认点击后进行排序）

		QStringList header;
		header<<tr("单号")<<tr("日期")<<tr("名称")<<tr("规格")<<tr("生产厂商")<<tr("单位")<<tr("调拨数量")<<tr("价格")<<tr("总价");
		ui.tableWidget->setHorizontalHeaderLabels(header);

		ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		ui.tableWidget->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: white;}");
		ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	

		if(ui.radioButton->isChecked())
		{
			int itotalNum=0;
			double dtotalPrice=0;
			strName = ui.lineEdit_DrugName->text();
			strsql= "select * from yk_warehouseallot where name='"+strName+"'and date between '"+strDate1+"' and '"+strDate2+"'";
			query.exec(strsql);
			iRow = 0;
			while(query.next())
			{
				int rows = ui.tableWidget->model()->rowCount();   //行总数
				ui.tableWidget->insertRow(rows);
				iRow=rows;
				ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(1).toString()));
				ui.tableWidget->setItem(iRow,1,new QTableWidgetItem(query.value(2).toString()));
				ui.tableWidget->setItem(iRow,2,new QTableWidgetItem(query.value(5).toString()));
				ui.tableWidget->setItem(iRow,3,new QTableWidgetItem(query.value(8).toString()));
				ui.tableWidget->setItem(iRow,4,new QTableWidgetItem(query.value(9).toString()));
				ui.tableWidget->setItem(iRow,5,new QTableWidgetItem(query.value(10).toString()));//单位
				ui.tableWidget->setItem(iRow,6,new QTableWidgetItem(query.value(12).toString()));//
				ui.tableWidget->setItem(iRow,7,new QTableWidgetItem(query.value(13).toString()));//
				ui.tableWidget->setItem(iRow,8,new QTableWidgetItem(query.value(14).toString()));//
				//ui.tableWidget->setItem(iRow,9,new QTableWidgetItem(query.value(15).toString()));//
				int num = query.value(12).toInt();
				itotalNum = itotalNum + num;
				double price = query.value(14).toDouble();
				dtotalPrice = dtotalPrice + price;
			}
			QString strProfit = QString::number(itotalNum);
			ui.lineEdit_TotalNum->setText(strProfit);
			strProfit = QString::number(dtotalPrice);
			ui.lineEdit_TotalPrice->setText(strProfit);
		}
		if(!ui.radioButton->isChecked())
		{
			int itotalNum=0;
			double dtotalPrice=0;
			strName = ui.lineEdit_DrugName->text();
			strsql= "select * from yk_warehouseallot where date between '"+strDate1+"' and '"+strDate2+"'";
			query.exec(strsql);
			iRow = 0;
			while(query.next())
			{
				int rows = ui.tableWidget->model()->rowCount();   //行总数
				ui.tableWidget->insertRow(rows);
				iRow=rows;
				ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(1).toString()));
				ui.tableWidget->setItem(iRow,1,new QTableWidgetItem(query.value(2).toString()));
				ui.tableWidget->setItem(iRow,2,new QTableWidgetItem(query.value(7).toString()));
				ui.tableWidget->setItem(iRow,3,new QTableWidgetItem(query.value(8).toString()));
				ui.tableWidget->setItem(iRow,4,new QTableWidgetItem(query.value(9).toString()));
				ui.tableWidget->setItem(iRow,5,new QTableWidgetItem(query.value(10).toString()));
				ui.tableWidget->setItem(iRow,6,new QTableWidgetItem(query.value(12).toString()));//
				ui.tableWidget->setItem(iRow,7,new QTableWidgetItem(query.value(13).toString()));//
				ui.tableWidget->setItem(iRow,8,new QTableWidgetItem(query.value(14).toString()));//
				//ui.tableWidget->setItem(iRow,9,new QTableWidgetItem(query.value(15).toString()));//
				//iRow ++;
				int num = query.value(12).toInt();
				itotalNum = itotalNum + num;
				double price = query.value(14).toDouble();
				dtotalPrice = dtotalPrice + price;
			}
			QString strProfit = QString::number(itotalNum);
			ui.lineEdit_TotalNum->setText(strProfit);
			strProfit = QString::number(dtotalPrice);
			ui.lineEdit_TotalPrice->setText(strProfit);
		}
	}
	else if(strStd == "调价记录")
	{
		ui.tableWidget->setColumnCount(11); //设置列数
		ui.tableWidget->horizontalHeader()->setClickable(false); //设置表头不可点击（默认点击后进行排序）

		QStringList header;
		header<<tr("单号")<<tr("日期")<<tr("编号")<<tr("名称")<<tr("规格")<<tr("生产厂商")<<tr("单位")<<tr("数量")<<tr("原价")<<tr("新价")<<tr("调价损益");
		ui.tableWidget->setHorizontalHeaderLabels(header);

		ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		ui.tableWidget->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: white;}");
		ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	

		if(ui.radioButton->isChecked())
		{
			int itotalNum=0;
			double dtotalPrice=0;
			strName = ui.lineEdit_DrugName->text();
			strsql= "select * from yk_pricing where name='"+strName+"'and date between '"+strDate1+"' and '"+strDate2+"'";
			query.exec(strsql);
			iRow = 0;
			while(query.next())
			{
				int rows = ui.tableWidget->model()->rowCount();   //行总数
				ui.tableWidget->insertRow(rows);
				iRow=rows;
				ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(1).toString()));
				ui.tableWidget->setItem(iRow,1,new QTableWidgetItem(query.value(2).toString()));
				ui.tableWidget->setItem(iRow,2,new QTableWidgetItem(query.value(4).toString()));
				ui.tableWidget->setItem(iRow,3,new QTableWidgetItem(query.value(5).toString()));
				ui.tableWidget->setItem(iRow,4,new QTableWidgetItem(query.value(6).toString()));
				ui.tableWidget->setItem(iRow,5,new QTableWidgetItem(query.value(7).toString()));
				ui.tableWidget->setItem(iRow,6,new QTableWidgetItem(query.value(8).toString()));//
				ui.tableWidget->setItem(iRow,7,new QTableWidgetItem(query.value(11).toString()));//
				ui.tableWidget->setItem(iRow,8,new QTableWidgetItem(query.value(9).toString()));//
				ui.tableWidget->setItem(iRow,9,new QTableWidgetItem(query.value(10).toString()));//
				ui.tableWidget->setItem(iRow,10,new QTableWidgetItem(query.value(12).toString()));//
				//ui.tableWidget->setItem(iRow,11,new QTableWidgetItem(query.value(12).toString()));//
				itotalNum = itotalNum + 1;
				double price = query.value(10).toDouble();
				dtotalPrice = dtotalPrice + price;
			}
			QString strProfit = QString::number(itotalNum);
			ui.lineEdit_TotalNum->setText(strProfit);
			strProfit = QString::number(dtotalPrice);
			ui.lineEdit_TotalPrice->setText(strProfit);
		}
		if(!ui.radioButton->isChecked())
		{
			int itotalNum=0;
			double dtotalPrice=0;
			strName = ui.lineEdit_DrugName->text();
			strsql= "select * from yk_pricing where date between '"+strDate1+"' and '"+strDate2+"'";
			query.exec(strsql);
			iRow = 0;
			while(query.next())
			{
				int rows = ui.tableWidget->model()->rowCount();   //行总数
				ui.tableWidget->insertRow(rows);
				iRow=rows;
				ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(1).toString()));
				ui.tableWidget->setItem(iRow,1,new QTableWidgetItem(query.value(2).toString()));
				ui.tableWidget->setItem(iRow,2,new QTableWidgetItem(query.value(4).toString()));
				ui.tableWidget->setItem(iRow,3,new QTableWidgetItem(query.value(5).toString()));
				ui.tableWidget->setItem(iRow,4,new QTableWidgetItem(query.value(6).toString()));
				ui.tableWidget->setItem(iRow,5,new QTableWidgetItem(query.value(7).toString()));
				ui.tableWidget->setItem(iRow,6,new QTableWidgetItem(query.value(8).toString()));//
				ui.tableWidget->setItem(iRow,7,new QTableWidgetItem(query.value(11).toString()));//
				ui.tableWidget->setItem(iRow,8,new QTableWidgetItem(query.value(9).toString()));//
				ui.tableWidget->setItem(iRow,9,new QTableWidgetItem(query.value(10).toString()));//
				ui.tableWidget->setItem(iRow,10,new QTableWidgetItem(query.value(12).toString()));//
				//ui.tableWidget->setItem(iRow,11,new QTableWidgetItem(query.value(12).toString()));//
				itotalNum = itotalNum + 1;
				double price = query.value(12).toDouble();
				dtotalPrice = dtotalPrice + price;
			}
			QString strProfit = QString::number(itotalNum);
			ui.lineEdit_TotalNum->setText(strProfit);
			strProfit = QString::number(dtotalPrice);
			ui.lineEdit_TotalPrice->setText(strProfit);
		}
	}
	else if(strStd == "盈亏损益")
	{
		ui.tableWidget->setColumnCount(11); //设置列数
		ui.tableWidget->horizontalHeader()->setClickable(false); //设置表头不可点击（默认点击后进行排序）

		QStringList header;
		header<<tr("单号")<<tr("日期")<<tr("名称")<<tr("规格")<<tr("生产厂商")<<tr("单位")<<tr("账面数量")<<tr("盘点数量")<<tr("盈亏数量")<<tr("价格")<<tr("损益金额");
		ui.tableWidget->setHorizontalHeaderLabels(header);

		ui.tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
		ui.tableWidget->setStyleSheet("QTableWidget{border: 1px solid gray;	background-color: transparent;	selection-color: white;}");
		ui.tableWidget->horizontalHeader()->setStyleSheet("QHeaderView::section {background-color:white;color: black;padding-left: 4px;border: 1px solid #6c6c6c;};"
		"color: white;padding-left: 4px;border: 1px solid #6c6c6c;}"
		"QHeaderView::section:checked{background-color: white;color: black;}");	

		if(ui.radioButton->isChecked())
		{
			int itotalNum=0;
			double dtotalPrice=0;
			strName = ui.lineEdit_DrugName->text();
			strsql= "select * from yk_check where name='"+strName+"'and date between '"+strDate1+"' and '"+strDate2+"'";
			query.exec(strsql);
			iRow = 0;
			while(query.next())
			{
				int rows = ui.tableWidget->model()->rowCount();   //行总数
				ui.tableWidget->insertRow(rows);
				iRow=rows;
				ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(1).toString()));
				ui.tableWidget->setItem(iRow,1,new QTableWidgetItem(query.value(2).toString()));
				ui.tableWidget->setItem(iRow,2,new QTableWidgetItem(query.value(6).toString()));
				ui.tableWidget->setItem(iRow,3,new QTableWidgetItem(query.value(7).toString()));
				ui.tableWidget->setItem(iRow,4,new QTableWidgetItem(query.value(8).toString()));
				ui.tableWidget->setItem(iRow,5,new QTableWidgetItem(query.value(11).toString()));//
				ui.tableWidget->setItem(iRow,6,new QTableWidgetItem(query.value(12).toString()));//
				ui.tableWidget->setItem(iRow,7,new QTableWidgetItem(query.value(13).toString()));//
				ui.tableWidget->setItem(iRow,8,new QTableWidgetItem(query.value(14).toString()));//
				ui.tableWidget->setItem(iRow,9,new QTableWidgetItem(query.value(15).toString()));//
				ui.tableWidget->setItem(iRow,10,new QTableWidgetItem(query.value(16).toString()));//
				int num = query.value(14).toInt();
				itotalNum = itotalNum + num;
				double price = query.value(16).toDouble();
				dtotalPrice = dtotalPrice + price;
			}
			QString strProfit = QString::number(itotalNum);
			ui.lineEdit_TotalNum->setText(strProfit);
			strProfit = QString::number(dtotalPrice);
			ui.lineEdit_TotalPrice->setText(strProfit);
		}
		if(!ui.radioButton->isChecked())
		{
			int itotalNum=0;
			double dtotalPrice=0;
			strName = ui.lineEdit_DrugName->text();
			strsql= "select * from yk_check where date between '"+strDate1+"' and '"+strDate2+"'";
			query.exec(strsql);
			iRow = 0;
			while(query.next())
			{
				int rows = ui.tableWidget->model()->rowCount();   //行总数
				ui.tableWidget->insertRow(rows);
				iRow=rows;
				ui.tableWidget->setItem(iRow,0,new QTableWidgetItem(query.value(1).toString()));
				ui.tableWidget->setItem(iRow,1,new QTableWidgetItem(query.value(2).toString()));
				ui.tableWidget->setItem(iRow,2,new QTableWidgetItem(query.value(6).toString()));
				ui.tableWidget->setItem(iRow,3,new QTableWidgetItem(query.value(7).toString()));
				ui.tableWidget->setItem(iRow,4,new QTableWidgetItem(query.value(8).toString()));
				ui.tableWidget->setItem(iRow,5,new QTableWidgetItem(query.value(11).toString()));//
				ui.tableWidget->setItem(iRow,6,new QTableWidgetItem(query.value(12).toString()));//
				ui.tableWidget->setItem(iRow,7,new QTableWidgetItem(query.value(13).toString()));//
				ui.tableWidget->setItem(iRow,8,new QTableWidgetItem(query.value(14).toString()));//
				ui.tableWidget->setItem(iRow,9,new QTableWidgetItem(query.value(15).toString()));//
				ui.tableWidget->setItem(iRow,10,new QTableWidgetItem(query.value(16).toString()));//
				//iRow ++;
				int num = query.value(14).toInt();
				itotalNum = itotalNum + num;
				double price = query.value(16).toDouble();
				dtotalPrice = dtotalPrice + price;
			}
			QString strProfit = QString::number(itotalNum);
			ui.lineEdit_TotalNum->setText(strProfit);
			strProfit = QString::number(dtotalPrice);
			ui.lineEdit_TotalPrice->setText(strProfit);
		}
	}
}
void DrugStatistics::on_printButton_clicked()
{
	QPrinter       printer( QPrinter::PrinterResolution );
	QPrintDialog   dialog( &printer, this );
	if ( dialog.exec() == QDialog::Accepted ) print( &printer );
}
void DrugStatistics::on_previewButton_clicked()
{
	filePrintPreview();
}
void DrugStatistics::on_excelButton_clicked()
{

	QAbstractItemModel* model=ui.tableWidget->model();
	if (model->rowCount()==0||model==NULL)
	{
		QString str = str.fromLocal8Bit("提示");
		QString str2 = str.fromLocal8Bit("无数据");
		QMessageBox box(QMessageBox::Warning,QString::fromLocal8Bit("警告"),str2);
		box.setStandardButtons (QMessageBox::Ok);
		box.setButtonText (QMessageBox::Ok,QString::fromLocal8Bit("确 定"));
		box.exec();
		return;
	}

	QFileDialog dlg;
	dlg.setAcceptMode(QFileDialog::AcceptSave);

	dlg.setDirectory(QDesktopServices::storageLocation(QDesktopServices::DesktopLocation));
	dlg.setNameFilter("*.xls");

	dlg.selectFile(ui.comboBox_find->currentText());
	if(dlg.exec()!= QDialog::Accepted)
		return;


	QString filePath=dlg.selectedFiles()[0];
	if(OdbcExcel::saveFromTable(filePath,ui.tableWidget,"")) {
		QString str = str.fromLocal8Bit("提示");
		QString str2 = str.fromLocal8Bit("保存成功");
		QMessageBox box(QMessageBox::Warning,QString::fromLocal8Bit("警告"),str2);
		box.setStandardButtons (QMessageBox::Ok);
		box.setButtonText (QMessageBox::Ok,QString::fromLocal8Bit("确 定"));
		box.exec();
	}
	else{
		QString str = str.fromLocal8Bit("错误");
		QString msg=str.fromLocal8Bit("保存失败！\n\r")+OdbcExcel::getError();
		QMessageBox box(QMessageBox::Warning,QString::fromLocal8Bit("警告"),msg);
		box.setStandardButtons (QMessageBox::Ok);
		box.setButtonText (QMessageBox::Ok,QString::fromLocal8Bit("确 定"));
		box.exec();
	}
}
void DrugStatistics::filePrintPreview()
{
	// 打印预览对话框
	QPrinter             printer( QPrinter::PrinterResolution );
	QPrintPreviewDialog  preview( &printer, this );
	preview.setMinimumSize(800,600);
	preview.setWindowTitle(QString::fromLocal8Bit("预览"));
	connect( &preview, SIGNAL(paintRequested(QPrinter*)), SLOT(print(QPrinter*)) );
	preview.exec();
}
void DrugStatistics::print( QPrinter* printer )
{
	QPainter painter( printer );
	int      w = printer->pageRect().width();
	int      h = printer->pageRect().height();
	QRect    page( w/50, h/50, w, h );
	QRect    page4( w/30, h/10, w, h );
	QFont    font = painter.font();
	font.setPixelSize( 50 );
	painter.setFont( font );
	QString	strFindItem = ui.comboBox_find->currentText();
	QString strtitle = QString::fromLocal8Bit(" 三河市燕郊镇卫生院")+strFindItem;
	painter.drawText( page, Qt::AlignTop    | Qt::AlignHCenter, strtitle);

	QPixmap image;
	image=image.grabWidget(ui.tableWidget,0,0,1000, 1000);
	//	painter.drawPixmap(page4,image);

	painter.begin(this);
	painter.setPen(QPen(Qt::black,4,Qt::SolidLine));//设置画笔形式 
	painter.setBrush(QBrush(Qt::white,Qt::SolidPattern));//设置画刷形式 
	int row = ui.tableWidget->rowCount();
	int col = ui.tableWidget->columnCount();
	if(col==0||row==0)return;
	double cellwidth = (w-40)/col;
	double cellheight = 160;
	double upmargin = 300;
	//计算总页数
	int firstpagerow = (h-800)/160;//第一页上方空白为750,下方为50
	int everypagerow = (h-100)/160;//后面每页的空白为100
	int pagecount = 0;
	//xp系统
	if(sql.windowsFlag==QSysInfo::WV_5_1||sql.windowsFlag==QSysInfo::WV_5_0||sql.windowsFlag==QSysInfo::WV_5_2||sql.windowsFlag==QSysInfo::WV_4_0)//判断当前系统
	{
		cellwidth= (w-100)/col;
		cellheight=60;
		upmargin = 50;
		firstpagerow = (h-200)/cellheight;
		everypagerow = (h-20)/cellheight;
	}
	if (row>firstpagerow)
	{
		pagecount = (row -firstpagerow)/everypagerow;
		int remain  = (row -firstpagerow)%everypagerow;
		if (remain!=0)
		{
			pagecount+=2;
		}
		else
		{
			pagecount+=1;
		}
	}
	else
	{
		pagecount=1;
	}
	if (pagecount == 1)
	{

		QStringList list;
		for (int j =0;j<col;j++)
		{
			list.append(ui.tableWidget->horizontalHeaderItem(j)->text());
		}
		for (int i=0;i<row;i++)
		{
			for (int j=0;j<col;j++)
			{
				if (ui.tableWidget->item(i,j)==NULL)
				{
					list.append("");
					continue;
				}
				list.append(ui.tableWidget->item(i,j)->text());
			}
		}
		for (int i=0;i<row+1;i++)
		{
			for (int j=0;j<col;j++)
			{
				painter.drawRect(20+j*cellwidth,upmargin+cellheight*(i+1),cellwidth,cellheight);
				QRect rect(20+j*cellwidth,upmargin+cellheight*(i+1),cellwidth,cellheight);
				painter.drawText( rect, Qt::AlignVCenter    | Qt::AlignHCenter, list.at(i*col+j) );//ui.tableWidget->item(i,j)->text()
			}
		}
		painter.end();
	}
	else
	{
		//首页
		QStringList list;
		for (int j =0;j<col;j++)
		{
			list.append(ui.tableWidget->horizontalHeaderItem(j)->text());
		}
		for (int i=0;i<firstpagerow;i++)
		{
			for (int j=0;j<col;j++)
			{
				if (ui.tableWidget->item(i,j)==NULL)
				{
					list.append("");
					continue;
				}
				list.append(ui.tableWidget->item(i,j)->text());
			}
		}
		for (int i=0;i<firstpagerow+1;i++)
		{
			for (int j=0;j<col;j++)
			{
				painter.drawRect(20+j*cellwidth,upmargin+cellheight*(i+1),cellwidth,cellheight);
				QRect rect(20+j*cellwidth,upmargin+cellheight*(i+1),cellwidth,cellheight);
				painter.drawText( rect, Qt::AlignVCenter    | Qt::AlignHCenter, list.at(i*col+j) );//ui.tableWidget->item(i,j)->text()
			}
		}
		printer->newPage();
		//占满的页面
		for (int k = 0;k<pagecount-2;k++)
		{
			list.clear();
			for (int i=firstpagerow+k*everypagerow;i<firstpagerow+(k+1)*everypagerow;i++)
			{
				for (int j=0;j<col;j++)
				{
					if (ui.tableWidget->item(i,j)==NULL)
					{
						list.append("");
						continue;
					}
					list.append(ui.tableWidget->item(i,j)->text());
				}
			}
			for (int i=0;i<everypagerow;i++)
			{
				for (int j=0;j<col;j++)
				{
					painter.drawRect(20+j*cellwidth,50+cellheight*(i),cellwidth,cellheight);
					QRect rect(20+j*cellwidth,50+cellheight*(i),cellwidth,cellheight);
					painter.drawText( rect, Qt::AlignVCenter    | Qt::AlignHCenter, list.at(i*col+j) );//ui.tableWidget->item(i,j)->text()
				}
			}
			printer->newPage();
		}
		//
		list.clear();
		for (int i=firstpagerow+(pagecount-2)*everypagerow;i<row;i++)
		{
			for (int j=0;j<col;j++)
			{
				if (ui.tableWidget->item(i,j)==NULL)
				{
					list.append("");
					continue;
				}
				list.append(ui.tableWidget->item(i,j)->text());
			}
		}
		for (int i=0;i<row-firstpagerow-(pagecount-2)*everypagerow;i++)
		{
			for (int j=0;j<col;j++)
			{
				painter.drawRect(20+j*cellwidth,50+cellheight*(i+1),cellwidth,cellheight);
				QRect rect(20+j*cellwidth,50+cellheight*(i+1),cellwidth,cellheight);
				painter.drawText( rect, Qt::AlignVCenter    | Qt::AlignHCenter, list.at(i*col+j) );//ui.tableWidget->item(i,j)->text()
			}
		}
		painter.end();
	}
} 
void DrugStatistics::completeText(const QModelIndex &index) {
	QString strName = index.data().toString();
	ui.lineEdit_DrugName->setText(strName);
	druglist->hide();
}
void DrugStatistics::keyPressEvent(QKeyEvent *e) {
	if (!druglist->isHidden()) {
		int key = e->key();
		int count = druglist->model()->rowCount();
		QModelIndex currentIndex = druglist->currentIndex();

		if (Qt::Key_Down == key) {
			// 按向下方向键时，移动光标选中下一个完成列表中的项
			int row = currentIndex.row() + 1;
			if (row >= count) {
				row = 0;
			}

			QModelIndex index = druglist->model()->index(row, 0);
			druglist->setCurrentIndex(index);
		} else if (Qt::Key_Up == key) {
			// 按向下方向键时，移动光标选中上一个完成列表中的项
			int row = currentIndex.row() - 1;
			if (row < 0) {
				row = count - 1;
			}

			QModelIndex index = druglist->model()->index(row, 0);
			druglist->setCurrentIndex(index);
		} else if (Qt::Key_Escape == key) {
			// 按下Esc键时，隐藏完成列表
			druglist->hide();
		} else if (Qt::Key_Enter == key || Qt::Key_Return == key) {
			// 按下回车键时，使用完成列表中选中的项，并隐藏完成列表
			if (currentIndex.isValid()) {
				QString text = druglist->currentIndex().data().toString();
				ui.lineEdit_DrugName->setText(text);
			}

			druglist->hide();
		} else {
			// 其他情况，隐藏完成列表，并使用QLineEdit的键盘按下事件
			druglist->hide();
			//QLineEdit::keyPressEvent(e);
		}
	} else {
		//QLineEdit::keyPressEvent(e);
	}
}