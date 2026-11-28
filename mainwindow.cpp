#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QMessageBox>
#include <QGuiApplication>
#include <QClipboard>
#include <QFileDialog>
#include <QDir>
#include <QElapsedTimer>
#include "descompacter.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QFile file(ui->lineEdit->text());
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Erro",
                             "Não foi possível abrir o arquivo:\n" + file.fileName());
        return;
    }

    wchar_t input_char;
    std::wstring input_string;

    QString text = QString::fromUtf8(file.readAll());

    if (text.isEmpty()) {
        QMessageBox::warning(this, "Erro", "O arquivo está vazio.");
        return;
    }

    huffman = HT::huffman(text.toStdWString());
    ui->tabWidget->setCurrentIndex(1);
    fill_table();
    fill_stats(file.size());
    fill_bits();
}

void MainWindow::fill_table()
{
    // if (huffman) return;

    const auto freqs = huffman.get_char_frequencies();
    const auto codes = huffman.get_huffman_codes();

    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(static_cast<int>(freqs.size()));

    int row = 0;
    for (const auto &[ch, freq] : freqs) {
        // Torna visíveis os caracteres invisíveis
        QString chText;
        switch (ch) {
        case L' ':  chText = "␣ (espaço)"; break;
        case L'\n': chText = "\\n";        break;
        case L'\t': chText = "\\t";        break;
        case L'\r': chText = "\\r";        break;
        default:    chText = QString::fromWCharArray(&ch, 1);
        }

        auto it = codes.find(ch);
        QString code = (it != codes.end()) ? QString::fromStdWString(it->second) : "";
        int bits = code.length();

        // Números via setData para a ordenação ser numérica, não alfabética
        auto *itemFreq  = new QTableWidgetItem;  itemFreq->setData(Qt::DisplayRole, freq);
        auto *itemBits  = new QTableWidgetItem;  itemBits->setData(Qt::DisplayRole, bits);
        auto *itemTotal = new QTableWidgetItem;  itemTotal->setData(Qt::DisplayRole, freq * bits);

        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(chText));
        ui->tableWidget->setItem(row, 1, itemFreq);
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem(code));
        ui->tableWidget->setItem(row, 3, itemBits);
        ui->tableWidget->setItem(row, 4, itemTotal);
        ++row;
    }

    ui->tableWidget->setSortingEnabled(true);
    ui->tableWidget->sortItems(1, Qt::DescendingOrder);   // mais frequentes primeiro
    // ui->tableWidget->resizeColumnsToContents();
}

void MainWindow::fill_stats(qint64 originalBytes)
{
    const auto freqs = huffman.get_char_frequencies();
    const auto codes = huffman.get_huffman_codes();

    // Total de bits comprimidos = soma de (frequência × tamanho do código)
    qint64 compressedBits = 0;
    for (const auto &[ch, freq] : freqs) {
        auto it = codes.find(ch);
        if (it != codes.end())
            compressedBits += static_cast<qint64>(freq) * it->second.size();
    }

    const qint64 originalBits    = originalBytes * 8;
    const qint64 compressedBytes = (compressedBits + 7) / 8;   // arredonda para cima
    const qint64 diffBytes       = originalBytes - compressedBytes;
    const double ratio = originalBytes > 0 ? 100.0 * diffBytes / originalBytes : 0.0;

    QLocale loc;   // formata com separador de milhar (1.234.567)
    ui->label_2->setText(QString("%1 bytes (%2 bits)")
                             .arg(loc.toString(originalBytes), loc.toString(originalBits)));
    ui->label_4->setText(QString("%1 bytes (%2 bits)")
                             .arg(loc.toString(compressedBytes), loc.toString(compressedBits)));
    ui->label_6->setText(QString("%1 bytes economizados (%2%)")
                             .arg(loc.toString(diffBytes))
                             .arg(ratio, 0, 'f', 2));
}

void MainWindow::fill_bits()
{
    encodedBits = QString::fromStdWString(huffman.get_encoded_text());

    constexpr qsizetype MAX_PREVIEW = 100'000;   // bits mostrados na tela
    const qsizetype n = qMin(encodedBits.size(), MAX_PREVIEW);

    // Formata: grupos de 8 bits separados por espaço, 8 grupos por linha
    QString preview;
    preview.reserve(n + n / 8 + 1);
    for (qsizetype i = 0; i < n; ++i) {
        preview += encodedBits[i];
        if ((i + 1) % 64 == 0)      preview += '\n';
        else if ((i + 1) % 8 == 0)  preview += ' ';
    }

    if (encodedBits.size() > MAX_PREVIEW)
        preview += QString("\n\n... (mostrando %1 de %2 bits — use \"Copiar\" para obter tudo)")
                       .arg(QLocale().toString(n), QLocale().toString(encodedBits.size()));

    ui->plainTextEdit_2->setReadOnly(true);
    ui->plainTextEdit_2->setPlainText(preview);
}


void MainWindow::on_pushButton_copy_clicked()
{
    if (encodedBits.isEmpty()) return;
    QGuiApplication::clipboard()->setText(encodedBits);
    QMessageBox::information(this, "Copiado",
                             QString("%1 bits copiados.").arg(QLocale().toString(encodedBits.size())));
}


void MainWindow::on_pushButtonopen_clicked()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        "Abrir arquivo de texto",
        QDir::homePath(),
        "Arquivos de texto (*.txt);;Todos os arquivos (*)");

    if (path.isEmpty()) return;   // usuário cancelou

    ui->lineEdit->setText(path);
    QFile file(path);
    if (file.open(QIODevice::ReadOnly)) {
        // lê só os primeiros 20 KB, para não travar com arquivos grandes
        // ui->plainTextEdit->setPlainText(QString::fromUtf8(file.read(20 * 1024)));
        ui->plainTextEdit->setPlainText(QString::fromUtf8(file.readAll()));
        ui->plainTextEdit->setReadOnly(true);
    }
}


void MainWindow::on_pushButton_openComp_clicked()
{
    const QString path = QFileDialog::getOpenFileName(
        this, "Abrir arquivo compactado", QDir::homePath(),
        "Arquivo compactado (*.tar);;Todos os arquivos (*)");
    if (!path.isEmpty())
        ui->lineEdit_compPath->setText(path);
}


void MainWindow::on_pushButton_descomp_clicked()
{
    const QString inPath = ui->lineEdit_compPath->text();
    if (!QFileInfo::exists(inPath)) {
        QMessageBox::warning(this, "Erro", "Selecione um arquivo .tar válido.");
        return;
    }

    const QString outPath = QFileDialog::getSaveFileName(
        this, "Salvar texto descompactado",
        QFileInfo(inPath).absolutePath() + "/arq_descompactado.txt",
        "Texto (*.txt)");
    if (outPath.isEmpty()) return;

    QApplication::setOverrideCursor(Qt::WaitCursor);
    QElapsedTimer timer;
    timer.start();

    HT::descompacter d;
    const int status = d.descompactar(inPath.toStdString(), outPath.toStdString());

    const qint64 ms = timer.elapsed();
    QApplication::restoreOverrideCursor();

    switch (status) {
    case 1: QMessageBox::critical(this, "Erro", "Falha ao ler a tabela de códigos (codificacao.txt)."); return;
    case 2: QMessageBox::critical(this, "Erro", "Falha ao ler os dados compactados (arq_compactado.txt)."); return;
    case 3: QMessageBox::critical(this, "Erro", "Não foi possível extrair o .tar."); return;
    }

    // Mostra o resultado
    QFile out(outPath);
    if (!out.open(QIODevice::ReadOnly)) return;
    const QByteArray data = out.readAll();

    ui->plainTextEdit_descomp->setPlainText(QString::fromUtf8(data.left(20 * 1024)));

    QLocale loc;
    const qint64 inSize = QFileInfo(inPath).size();
    ui->label_descompInfo->setText(
        QString("Compactado: %1  →  Descompactado: %2  |  Tempo: %3 ms")
            .arg(loc.formattedDataSize(inSize), loc.formattedDataSize(data.size()))
            .arg(ms));

    statusBar()->showMessage("Descompactado em " + outPath, 5000);
}

