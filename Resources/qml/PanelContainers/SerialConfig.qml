import QtQuick 1.1
import "../Controls"
import "../Panels/Config"

Item{
    signal readyOpenSerialPort(variant config)

    property Panel panel
    property Stack stack
    id: serialConfig
    panel: serialConfigPanel
    objectName: "serialConfig"

    function prepareOpenSerialPort()
    {
        console.log("prepareOpenSerialPort()");
         serialConfig.readyOpenSerialPort(config)
    }

    resources:
    [
        QtObject
        {
            id: config
            property string portNo: portNoPanel.selectedContent
            property string baudRate: baudRatePanel.selectedContent
            property string dataBits: dataBitsPanel.selectedContent
            property  string stopBits:  stopBitsPanel.selectedContent
            property string parity: parityPanel.selectedContent
            property string timeOut: timeOutPanel.selectedContent
        }

    ]

    SerialConfigPanel
    {
        id: serialConfigPanel
        stack: serialConfig.stack
        portNoPanel: portNoPanel
        baudRatePanel: baudRatePanel
        dataBitsPanel: dataBitsPanel
        stopBitsPanel: stopBitsPanel
        parityPanel: parityPanel
        timeOutPanel:  timeOutPanel
        onOpenSerialPort: { serialConfig.prepareOpenSerialPort()}
    }

    PortNoPanel
    {
        id: portNoPanel
        stack: serialConfig.stack
        onContentUpdated: { stack.addPanel(baudRatePanel); }
    }

    BaudRatePanel
    {
        id: baudRatePanel
        stack: serialConfig.stack
          onContentUpdated: { stack.addPanel(dataBitsPanel); }
    }

    DataBitsPanel
    {
        id: dataBitsPanel
        stack: serialConfig.stack
          onContentUpdated: { stack.addPanel(stopBitsPanel); }
    }

    StopBitsPanel
    {
        id: stopBitsPanel
        stack: serialConfig.stack
          onContentUpdated: { stack.addPanel(parityPanel); }
    }

    ParityPanel
    {
        id: parityPanel
        stack: serialConfig.stack
        onContentUpdated: {stack.addPanel(timeOutPanel);}
    }

    TimeOutPanel
    {
        id: timeOutPanel
        stack: serialConfig.stack
    }
}
