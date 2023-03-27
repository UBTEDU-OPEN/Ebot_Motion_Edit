#include "mp3container.h"
#include "ui_mp3container.h"

mp3Container::mp3Container(QWidget *parent) :
    UBXWidget(parent),
    ui(new Ui::mp3Container)
{
    ui->setupUi(this);
    m_pMp3Widget = new MP3Widget(this);
    m_pScrollbar = new QScrollBar(Qt::Horizontal);
    m_pScrollbar->setFixedHeight(18);
    ui->verticalLayout->addWidget(m_pMp3Widget);
    ui->verticalLayout->addWidget(m_pScrollbar);
    connect(m_pScrollbar, &QScrollBar::valueChanged, m_pMp3Widget, &MP3Widget::OnScrollBarValueChanged);
    connect(m_pMp3Widget, &MP3Widget::SigRangeChanged, this, &mp3Container::OnRangeChanged);
    connect(m_pMp3Widget, &MP3Widget::SigCellIndexChanged, this, &mp3Container::OnCellIndexChanged);
}

mp3Container::~mp3Container()
{
    delete ui;
}

void mp3Container::OnRangeChanged(int min, int max, int nStep)
{
    m_pScrollbar->setRange(min, max);
    if(m_pScrollbar->singleStep() != nStep)
    {
        m_pScrollbar->setSingleStep(nStep);
        m_pScrollbar->setPageStep(nStep);
    }
}

void mp3Container::OnCellIndexChanged(int index)
{
    int nStep = m_pScrollbar->pageStep();
    m_pScrollbar->setValue(index * nStep);
}

void mp3Container::wheelEvent(QWheelEvent *event)
{
    if(NULL != m_pScrollbar)
    {
        QWheelEvent *glevent=new QWheelEvent(event->pos(),
                                             event->globalPos(),
                                             event->pixelDelta(),
                                             event->angleDelta(),
                                             event->delta(),
                                             event->orientation(),
                                             event->buttons(),
                                             event->modifiers(),
                                             event->phase(),
                                             event->source());
        qApp->postEvent(m_pScrollbar, glevent);
    }
}
