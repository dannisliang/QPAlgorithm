//
// Created by andy_ro@qq.com
// 			5/26/2019
//

#include "pb2Json.h"

namespace PB2JSON {

	void Pb2Json::PbMsg2JsonStr(ProtobufMessage const& src, std::string& dst, bool enum2str) {
		Json::Value value;
		PbMsg2Json(src, value, enum2str);
		Json::FastWriter writer;
		dst = writer.write(value);
	}

	bool Pb2Json::JsonStr2PbMsg(std::string const& src, ProtobufMessage& dst, bool str2enum) {
		Json::Value value;
		Json::Reader reader(Json::Features::strictMode());
		if (!reader.parse(src, value)) {
			printf("parse json string is fail,str=%s\n", src.c_str());
			return false;
		}
		if (!Json2PbMsg(value, dst, str2enum)) {
			printf("pb convert error");
			return false;
		}
		return true;
	}

	bool Pb2Json::Json2PbMsg(Json::Value const& src, ProtobufMessage& dst, bool str2enum) {
		ProtobufDescriptor const* descriptor = dst.GetDescriptor();
		ProtobufReflection const* reflection = dst.GetReflection();
		if (NULL == descriptor || NULL == reflection) {
			return false;
		}
		int32_t count = descriptor->field_count();
		for (int32_t i = 0; i < count; ++i) {
			ProtobufFieldDescriptor const* field = descriptor->field(i);
			if (NULL == field) {
				continue;
			}
			if (!src.isMember(field->name())) {
				continue;
			}
			Json::Value const& value = src[field->name()];
			if (field->is_repeated()) {
				if (!value.isArray()) {
					printf("pb error");
					return false;
				}
				else {
					Json2RepeatedMessage(value, dst, field, reflection, str2enum);
					continue;
				}
			}
			switch (field->type()) {
			case ProtobufFieldDescriptor::TYPE_BOOL: {
				if (value.isBool()) {
					reflection->SetBool(&dst, field, value.asBool());
				}
				else if (value.isInt()) {
					reflection->SetBool(&dst, field, value.isInt());
				}
				else if (value.isString()) {
					if (value.asString() == "true") {
						reflection->SetBool(&dst, field, true);
					}
					else if (value.asString() == "false") {
						reflection->SetBool(&dst, field, false);
					}
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_INT32:
			case ProtobufFieldDescriptor::TYPE_SINT32:
			case ProtobufFieldDescriptor::TYPE_SFIXED32: {
				if (value.isInt()) {
					reflection->SetInt32(&dst, field, value.asInt());
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_UINT32:
			case ProtobufFieldDescriptor::TYPE_FIXED32: {
				if (value.isUInt()) {
					reflection->SetUInt32(&dst, field, value.asUInt());
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_INT64:
			case ProtobufFieldDescriptor::TYPE_SINT64:
			case ProtobufFieldDescriptor::TYPE_SFIXED64: {
				if (value.isInt()) {
					reflection->SetInt64(&dst, field, value.asInt64());
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_UINT64:
			case ProtobufFieldDescriptor::TYPE_FIXED64: {
				if (value.isUInt()) {
					reflection->SetUInt64(&dst, field, value.asUInt64());
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_FLOAT: {
				if (value.isDouble()) {
					reflection->SetFloat(&dst, field, value.asFloat());
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_DOUBLE: {
				if (value.isDouble()) {
					reflection->SetDouble(&dst, field, value.asDouble());
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_STRING:
			case ProtobufFieldDescriptor::TYPE_BYTES: {
				if (value.isString()) {
					reflection->SetString(&dst, field, value.asString());
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_MESSAGE: {
				if (value.isObject()) {
					Json2PbMsg(value, *reflection->MutableMessage(&dst, field));
				}
				break;
			}
			}
		}
		return true;
	}
	
	void Pb2Json::PbMsg2Json(
		ProtobufMessage const& src, Json::Value& dst, bool enum2str) {
		ProtobufDescriptor const* descriptor = src.GetDescriptor();
		ProtobufReflection const* reflection = src.GetReflection();
		if (NULL == descriptor || NULL == descriptor) {
			return;
		}
		int32_t count = descriptor->field_count();
		for (int32_t i = 0; i < count; ++i) {
			ProtobufFieldDescriptor const* field = descriptor->field(i);
			if (field->is_repeated()) {
				if (reflection->FieldSize(src, field) > 0) {
					RepeatedMessage2Json(src, field, reflection, dst[field->name()], enum2str);
				}
				continue;
			}
			if (!reflection->HasField(src, field)) {
				continue;
			}
			switch (field->type()) {
			case ProtobufFieldDescriptor::TYPE_MESSAGE: {
				ProtobufMessage const& tMsg = reflection->GetMessage(src, field);
				if (0 != tMsg.ByteSize()) {
					PbMsg2Json(tMsg, dst[field->name()]);
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_BOOL:
				dst[field->name()] = reflection->GetBool(src, field) ? true : false;
				break;
			case ProtobufFieldDescriptor::TYPE_ENUM: {
				::google::protobuf::EnumValueDescriptor const* enum_value_desc = reflection->GetEnum(src, field);
				if (enum2str) {
					dst[field->name()] = enum_value_desc->name();
				}
				else {
					dst[field->name()] = enum_value_desc->number();
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_INT32:
			case ProtobufFieldDescriptor::TYPE_SINT32:
			case ProtobufFieldDescriptor::TYPE_SFIXED32:
				dst[field->name()] = Json::Int(reflection->GetInt32(src, field));
				break;
			case ProtobufFieldDescriptor::TYPE_UINT32:
			case ProtobufFieldDescriptor::TYPE_FIXED32:
				dst[field->name()] = Json::UInt(reflection->GetUInt32(src, field));
				break;

			case ProtobufFieldDescriptor::TYPE_INT64:
			case ProtobufFieldDescriptor::TYPE_SINT64:
			case ProtobufFieldDescriptor::TYPE_SFIXED64:
				dst[field->name()] = Json::Int64(reflection->GetInt64(src, field));
				break;
			case ProtobufFieldDescriptor::TYPE_UINT64:
			case ProtobufFieldDescriptor::TYPE_FIXED64:
				dst[field->name()] = Json::UInt64(reflection->GetUInt64(src, field));
				break;
			case ProtobufFieldDescriptor::TYPE_FLOAT:
				dst[field->name()] = reflection->GetFloat(src, field);
				break;
			case ProtobufFieldDescriptor::TYPE_STRING:
			case ProtobufFieldDescriptor::TYPE_BYTES:
				dst[field->name()] = reflection->GetString(src, field);
				break;
			}
		}
	}
	
	bool Pb2Json::Json2RepeatedMessage(
		Json::Value const& json, ProtobufMessage& message,
		ProtobufFieldDescriptor const* field,
		ProtobufReflection const* reflection,
		bool str2enum) {
		int32_t count = json.size();
		for (int32_t j = 0; j < count; ++j) {
			switch (field->type()) {
			case ProtobufFieldDescriptor::TYPE_BOOL: {
				if (json.isBool()) {
					reflection->AddBool(&message, field, json[j].asBool());
				}
				else if (json[j].isInt()) {
					reflection->AddBool(&message, field, json[j].asInt());
				}
				else if (json[j].isString()) {
					if (json[j].asString() == "true") {
						reflection->AddBool(&message, field, true);
					}
					else if (json[j].asString() == "false") {
						reflection->AddBool(&message, field, false);
					}
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_ENUM: {
				::google::protobuf::EnumDescriptor const* pedesc = field->enum_type();
				::google::protobuf::EnumValueDescriptor const* pevdesc = NULL;
				if (str2enum) {
					pevdesc = pedesc->FindValueByName(json[j].asString());
				}
				else {
					pevdesc = pedesc->FindValueByNumber(json[j].asInt());
				}
				if (NULL != pevdesc) {
					reflection->AddEnum(&message, field, pevdesc);
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_INT32:
			case ProtobufFieldDescriptor::TYPE_SINT32:
			case ProtobufFieldDescriptor::TYPE_SFIXED32: {
				if (json[j].isInt()) {
					reflection->AddInt32(&message, field, json[j].asInt());
				}
			} break;
			case ProtobufFieldDescriptor::TYPE_UINT32:
			case ProtobufFieldDescriptor::TYPE_FIXED32: {
				if (json[j].isUInt()) {
					reflection->AddUInt32(&message, field, json[j].asUInt());
				}
			} break;
			case ProtobufFieldDescriptor::TYPE_INT64:
			case ProtobufFieldDescriptor::TYPE_SINT64:
			case ProtobufFieldDescriptor::TYPE_SFIXED64: {
				if (json[j].isInt()) {
					reflection->AddInt64(&message, field, json[j].asInt64());
				}
			} break;
			case ProtobufFieldDescriptor::TYPE_UINT64:
			case ProtobufFieldDescriptor::TYPE_FIXED64: {
				if (json[j].isUInt()) {
					reflection->AddUInt64(&message, field, json[j].asUInt64());
				}
			} break;
			case ProtobufFieldDescriptor::TYPE_FLOAT: {
				if (json[j].isDouble()) {
					reflection->AddFloat(&message, field, json[j].asFloat());
				}
			} break;
			case ProtobufFieldDescriptor::TYPE_DOUBLE: {
				if (json[j].isDouble()) {
					reflection->AddDouble(&message, field, json[j].asDouble());
				}
			} break;
			case ProtobufFieldDescriptor::TYPE_MESSAGE: {
				if (json[j].isObject()) {
					Json2PbMsg(json[j], *reflection->AddMessage(&message, field));
				}
			} break;
			case ProtobufFieldDescriptor::TYPE_STRING:
			case ProtobufFieldDescriptor::TYPE_BYTES: {
				if (json[j].isString()) {
					reflection->AddString(&message, field, json[j].asString());
				}
			} break;
			}
		}
		return true;
	}
	
	void Pb2Json::RepeatedMessage2Json(ProtobufMessage const& message,
		ProtobufFieldDescriptor const* field,
		ProtobufReflection const* reflection,
		Json::Value& json, bool enum2str)
	{
		if (NULL == field || NULL == reflection) {
			PbMsg2Json(message, json);
		}
		for (int32_t i = 0; i < reflection->FieldSize(message, field); ++i) {
			Json::Value tJson;
			switch (field->type()) {
			case ProtobufFieldDescriptor::TYPE_MESSAGE: {
				ProtobufMessage const& tMsg = reflection->GetRepeatedMessage(message, field, i);
				if (0 != tMsg.ByteSize()) {
					PbMsg2Json(tMsg, tJson);
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_BOOL:
				tJson[field->name()] = reflection->GetRepeatedBool(message, field, i) ? true : false;
				break;
			case ProtobufFieldDescriptor::TYPE_ENUM: {
				const ::google::protobuf::EnumValueDescriptor* enum_value_desc = reflection->GetRepeatedEnum(message, field, i);
				if (enum2str) {
					tJson = enum_value_desc->name();
				}
				else {
					tJson = enum_value_desc->number();
				}
				break;
			}
			case ProtobufFieldDescriptor::TYPE_INT32:
			case ProtobufFieldDescriptor::TYPE_SINT32:
			case ProtobufFieldDescriptor::TYPE_SFIXED32:
				tJson[field->name()] = reflection->GetRepeatedInt32(message, field, i);
				break;
			case ProtobufFieldDescriptor::TYPE_UINT32:
			case ProtobufFieldDescriptor::TYPE_FIXED32:
				tJson[field->name()] = reflection->GetRepeatedUInt32(message, field, i);
				break;
			case ProtobufFieldDescriptor::TYPE_INT64:
			case ProtobufFieldDescriptor::TYPE_SINT64:
			case ProtobufFieldDescriptor::TYPE_SFIXED64:
				tJson[field->name()] = (Json::Int64)reflection->GetRepeatedInt64(message, field, i);
				break;
			case ProtobufFieldDescriptor::TYPE_UINT64:
			case ProtobufFieldDescriptor::TYPE_FIXED64:
				tJson[field->name()] = Json::UInt64(reflection->GetRepeatedUInt64(message, field, i));
				break;
			case ProtobufFieldDescriptor::TYPE_FLOAT:
				tJson[field->name()] = reflection->GetRepeatedFloat(message, field, i);
				break;
			case ProtobufFieldDescriptor::TYPE_STRING:
			case ProtobufFieldDescriptor::TYPE_BYTES:
				tJson[field->name()] = reflection->GetRepeatedString(message, field, i);
				break;
			}
			json.append(tJson);
		}
	}
};