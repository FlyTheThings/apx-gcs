import qbs.FileInfo
import apx.Application as APX

APX.ApxLibrary {

    Depends { name: "ApxCore" }

    Depends {
        name: "Qt";
        submodules: [
            "core",
            "sql",
            "qml",
            "xml",
            "widgets",
        ]
    }

    Export {
        Depends { name: "cpp" }
        cpp.includePaths: product.sourceDirectory
    }

    Depends { name: "apx" }
    Depends { name: "cpp" }
    Depends { name: "sdk" }

    cpp.includePaths: base.concat([
        sourceDirectory,
        project.libDir,
    ])

    Group {
        name: "Lib"
        prefix: FileInfo.joinPaths(project.libDir, "/")
        files: [
            "Mandala/Mandala.cpp", "Mandala/Mandala.h",
            "Mandala/MandalaCore.cpp", "Mandala/MandalaCore.h",
            "Mandala/MatrixMath.cpp", "Mandala/MatrixMath.h",
            "Mandala/MandalaTemplate.h",
            "Mandala/MandalaIndexes.h",
            "Mandala/MandalaConstants.h",
            "Mandala/preprocessor.h",

            "ApxLink/crc.c", "ApxLink/crc.h",
            "ApxLink/escaped.h",
            "ApxLink/Mission.h",
            "ApxLink/node.h",
            "ApxLink/vehicle.h",
            //"ApxLink/packet.h",
            "ApxLink/types.h",
            "ApxLink/tcp_ports.h",

            //"comm.cpp", "comm.h",
            //"crc.h",
            //"dmsg.h",
            //"time_ms.h",
        ]
    }

    Group {
        name: "Dictionary"
        prefix: name+"/"
        files: [
            "DictMandala.cpp", "DictMandala.h",
            "DictNode.cpp", "DictNode.h",
            "DictMission.cpp", "DictMission.h",
            "MandalaIndex.h",
        ]
    }

    Group {
        name: "Protocols"
        prefix: name+"/"
        files: [
            "ApxProtocol.cpp", "ApxProtocol.h",
            "ProtocolBase.cpp", "ProtocolBase.h",
            "ProtocolVehicles.cpp", "ProtocolVehicles.h",
            "ProtocolVehicle.cpp", "ProtocolVehicle.h",
            "ProtocolTelemetry.cpp", "ProtocolTelemetry.h",
            "ProtocolMission.cpp", "ProtocolMission.h",
            "ProtocolService.cpp", "ProtocolService.h",
            "ProtocolServiceRequest.cpp", "ProtocolServiceRequest.h",
            "ProtocolServiceNode.cpp", "ProtocolServiceNode.h",
            "ProtocolServiceFile.cpp", "ProtocolServiceFile.h",
            "ProtocolServiceFirmware.cpp", "ProtocolServiceFirmware.h",
        ]
    }

    Group {
        name: "Database"
        prefix: name+"/"
        files: [
            "Database.cpp", "Database.h",
            "DatabaseSession.cpp", "DatabaseSession.h",
            "DatabaseWorker.cpp", "DatabaseWorker.h",
            "DatabaseRequest.cpp", "DatabaseRequest.h",
            "DatabaseLookup.cpp", "DatabaseLookup.h",
            "DatabaseLookupModel.cpp", "DatabaseLookupModel.h",
            "NodesDB.cpp", "NodesDB.h",
            "NodesReqDict.cpp", "NodesReqDict.h",
            "NodesReqNconf.cpp", "NodesReqNconf.h",
            "NodesReqVehicle.cpp", "NodesReqVehicle.h",
            "MissionsDB.cpp", "MissionsDB.h",
            "TelemetryDB.cpp", "TelemetryDB.h",
            "TelemetryReqWrite.cpp", "TelemetryReqWrite.h",
            "TelemetryReqRead.cpp", "TelemetryReqRead.h",
        ]
    }

    Group {
        name: "Sharing"
        prefix: name+"/"
        files: [
            "Share.cpp", "Share.h",
            "ShareXml.cpp", "ShareXml.h",
            "NodesXml.cpp", "NodesXml.h",
            "MissionsXml.cpp", "MissionsXml.h",
            "TelemetryXmlImport.cpp", "TelemetryXmlImport.h",
            "TelemetryXmlExport.cpp", "TelemetryXmlExport.h",
        ]
    }

}
