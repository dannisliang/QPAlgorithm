//
// Created by YangZhi
// 			5/26/2019
//
#ifndef PROTOBUF2JSON_H
#define PROTOBUF2JSON_H

#include <json/json.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

//protobuf与json之间转换
namespace PB2JSON {
	class Pb2Json {
	public:
		typedef ::google::protobuf::Message         ProtobufMessage;
		typedef ::google::protobuf::Reflection      ProtobufReflection;
		typedef ::google::protobuf::FieldDescriptor ProtobufFieldDescriptor;
		typedef ::google::protobuf::Descriptor      ProtobufDescriptor;
	public:
		static void PbMsg2JsonStr(ProtobufMessage const& src, std::string& dst, bool enum2str = false);
		static bool JsonStr2PbMsg(std::string const& src, ProtobufMessage& dst, bool str2enum = false);
		static bool Json2PbMsg(Json::Value const& src, ProtobufMessage& dst, bool str2enum = false);
		static void PbMsg2Json(ProtobufMessage const& src, Json::Value& dst, bool enum2str = false);
	private:
		static bool Json2RepeatedMessage(
			Json::Value const& json, ProtobufMessage& message,
			ProtobufFieldDescriptor const* field,
			ProtobufReflection const* reflection,
			bool str2enum = false);
		static void RepeatedMessage2Json(
			ProtobufMessage const& message,
			ProtobufFieldDescriptor const* field,
			ProtobufReflection const* reflection,
			Json::Value& json, bool enum2str);
	};
};

#endif