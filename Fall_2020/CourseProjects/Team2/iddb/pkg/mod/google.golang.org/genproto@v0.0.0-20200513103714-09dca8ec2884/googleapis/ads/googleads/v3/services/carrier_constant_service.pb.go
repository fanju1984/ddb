// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/ads/googleads/v3/services/carrier_constant_service.proto

package services

import (
	context "context"
	fmt "fmt"
	math "math"

	proto "github.com/golang/protobuf/proto"
	resources "google.golang.org/genproto/googleapis/ads/googleads/v3/resources"
	_ "google.golang.org/genproto/googleapis/api/annotations"
	grpc "google.golang.org/grpc"
	codes "google.golang.org/grpc/codes"
	status "google.golang.org/grpc/status"
)

// Reference imports to suppress errors if they are not otherwise used.
var _ = proto.Marshal
var _ = fmt.Errorf
var _ = math.Inf

// This is a compile-time assertion to ensure that this generated file
// is compatible with the proto package it is being compiled against.
// A compilation error at this line likely means your copy of the
// proto package needs to be updated.
const _ = proto.ProtoPackageIsVersion3 // please upgrade the proto package

// Request message for [CarrierConstantService.GetCarrierConstant][google.ads.googleads.v3.services.CarrierConstantService.GetCarrierConstant].
type GetCarrierConstantRequest struct {
	// Required. Resource name of the carrier constant to fetch.
	ResourceName         string   `protobuf:"bytes,1,opt,name=resource_name,json=resourceName,proto3" json:"resource_name,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *GetCarrierConstantRequest) Reset()         { *m = GetCarrierConstantRequest{} }
func (m *GetCarrierConstantRequest) String() string { return proto.CompactTextString(m) }
func (*GetCarrierConstantRequest) ProtoMessage()    {}
func (*GetCarrierConstantRequest) Descriptor() ([]byte, []int) {
	return fileDescriptor_3024993599d2a450, []int{0}
}

func (m *GetCarrierConstantRequest) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_GetCarrierConstantRequest.Unmarshal(m, b)
}
func (m *GetCarrierConstantRequest) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_GetCarrierConstantRequest.Marshal(b, m, deterministic)
}
func (m *GetCarrierConstantRequest) XXX_Merge(src proto.Message) {
	xxx_messageInfo_GetCarrierConstantRequest.Merge(m, src)
}
func (m *GetCarrierConstantRequest) XXX_Size() int {
	return xxx_messageInfo_GetCarrierConstantRequest.Size(m)
}
func (m *GetCarrierConstantRequest) XXX_DiscardUnknown() {
	xxx_messageInfo_GetCarrierConstantRequest.DiscardUnknown(m)
}

var xxx_messageInfo_GetCarrierConstantRequest proto.InternalMessageInfo

func (m *GetCarrierConstantRequest) GetResourceName() string {
	if m != nil {
		return m.ResourceName
	}
	return ""
}

func init() {
	proto.RegisterType((*GetCarrierConstantRequest)(nil), "google.ads.googleads.v3.services.GetCarrierConstantRequest")
}

func init() {
	proto.RegisterFile("google/ads/googleads/v3/services/carrier_constant_service.proto", fileDescriptor_3024993599d2a450)
}

var fileDescriptor_3024993599d2a450 = []byte{
	// 418 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x84, 0x52, 0x3d, 0xcb, 0xd3, 0x40,
	0x1c, 0x27, 0x11, 0x04, 0x83, 0x2e, 0x19, 0xb4, 0x4d, 0x05, 0x4b, 0x29, 0x52, 0x3a, 0xdc, 0x49,
	0xb3, 0xc8, 0x15, 0x95, 0x6b, 0x87, 0x3a, 0x49, 0x51, 0xec, 0x20, 0x81, 0x70, 0x4d, 0xce, 0x78,
	0x90, 0xdc, 0xd5, 0xbb, 0x34, 0x8b, 0xb8, 0xf4, 0x2b, 0xf8, 0x0d, 0x1c, 0xfd, 0x1e, 0x2e, 0x5d,
	0xdd, 0x9c, 0x1c, 0x9c, 0x9e, 0x8f, 0xf0, 0xc0, 0x03, 0x0f, 0xe9, 0xe5, 0xd2, 0x97, 0xa7, 0xa1,
	0xdb, 0x8f, 0xfb, 0xbd, 0xfc, 0xdf, 0xce, 0x79, 0x93, 0x08, 0x91, 0xa4, 0x14, 0x92, 0x58, 0x41,
	0x0d, 0x4b, 0x54, 0xf8, 0x50, 0x51, 0x59, 0xb0, 0x88, 0x2a, 0x18, 0x11, 0x29, 0x19, 0x95, 0x61,
	0x24, 0xb8, 0xca, 0x09, 0xcf, 0xc3, 0x8a, 0x01, 0x2b, 0x29, 0x72, 0xe1, 0x76, 0xb5, 0x0b, 0x90,
	0x58, 0x81, 0x3a, 0x00, 0x14, 0x3e, 0x30, 0x01, 0xde, 0xcb, 0xa6, 0x12, 0x92, 0x2a, 0xb1, 0x96,
	0xe7, 0x6a, 0xe8, 0x6c, 0xef, 0xa9, 0x71, 0xae, 0x18, 0x24, 0x9c, 0x8b, 0x9c, 0xe4, 0x4c, 0x70,
	0x55, 0xb1, 0x4f, 0x0e, 0xd8, 0x28, 0x65, 0xb4, 0xb6, 0x3d, 0x3b, 0x20, 0x3e, 0x33, 0x9a, 0xc6,
	0xe1, 0x92, 0x7e, 0x21, 0x05, 0x13, 0xb2, 0x12, 0xb4, 0x0f, 0x04, 0xa6, 0x09, 0x4d, 0xf5, 0xa4,
	0xd3, 0x9e, 0xd1, 0x7c, 0xaa, 0xfb, 0x99, 0x56, 0xed, 0xbc, 0xa7, 0x5f, 0xd7, 0x54, 0xe5, 0xee,
	0x47, 0xe7, 0x91, 0x91, 0x87, 0x9c, 0x64, 0xb4, 0x65, 0x75, 0xad, 0xc1, 0x83, 0xc9, 0x8b, 0x7f,
	0xd8, 0xbe, 0xc6, 0x43, 0x67, 0xb0, 0x9f, 0xbf, 0x42, 0x2b, 0xa6, 0x40, 0x24, 0x32, 0x78, 0x9a,
	0xf7, 0xd0, 0xc4, 0xbc, 0x23, 0x19, 0x1d, 0xdd, 0x58, 0xce, 0xe3, 0x13, 0xc5, 0x07, 0xbd, 0x3c,
	0xf7, 0xb7, 0xe5, 0xb8, 0x77, 0xfb, 0x71, 0xc7, 0xe0, 0xd2, 0xd6, 0x41, 0xe3, 0x14, 0xde, 0xa8,
	0xd1, 0x5c, 0x1f, 0x04, 0x9c, 0x58, 0x7b, 0xaf, 0xff, 0xe2, 0xe3, 0xd1, 0x37, 0x7f, 0xfe, 0xff,
	0xb0, 0x07, 0xee, 0xf3, 0xf2, 0x8e, 0xdf, 0x8e, 0x98, 0x57, 0xd1, 0xb1, 0x57, 0xc1, 0xe1, 0x77,
	0xaf, 0xb3, 0xc5, 0xad, 0xa6, 0xed, 0x4c, 0x36, 0xb6, 0xd3, 0x8f, 0x44, 0x76, 0x71, 0xa6, 0x49,
	0xe7, 0xfc, 0x96, 0xe6, 0xe5, 0xe5, 0xe6, 0xd6, 0xa7, 0xb7, 0x55, 0x40, 0x22, 0x52, 0xc2, 0x13,
	0x20, 0x64, 0x02, 0x13, 0xca, 0x77, 0x77, 0x85, 0xfb, 0x92, 0xcd, 0x5f, 0x7d, 0x6c, 0xc0, 0x4f,
	0xfb, 0xde, 0x0c, 0xe3, 0x5f, 0x76, 0x77, 0xa6, 0x03, 0x71, 0xac, 0x80, 0x86, 0x25, 0x5a, 0xf8,
	0xa0, 0x2a, 0xac, 0xb6, 0x46, 0x12, 0xe0, 0x58, 0x05, 0xb5, 0x24, 0x58, 0xf8, 0x81, 0x91, 0x5c,
	0xd9, 0x7d, 0xfd, 0x8e, 0x10, 0x8e, 0x15, 0x42, 0xb5, 0x08, 0xa1, 0x85, 0x8f, 0x90, 0x91, 0x2d,
	0xef, 0xef, 0xfa, 0xf4, 0x6f, 0x03, 0x00, 0x00, 0xff, 0xff, 0x1d, 0x75, 0x85, 0xb0, 0x91, 0x03,
	0x00, 0x00,
}

// Reference imports to suppress errors if they are not otherwise used.
var _ context.Context
var _ grpc.ClientConnInterface

// This is a compile-time assertion to ensure that this generated file
// is compatible with the grpc package it is being compiled against.
const _ = grpc.SupportPackageIsVersion6

// CarrierConstantServiceClient is the client API for CarrierConstantService service.
//
// For semantics around ctx use and closing/ending streaming RPCs, please refer to https://godoc.org/google.golang.org/grpc#ClientConn.NewStream.
type CarrierConstantServiceClient interface {
	// Returns the requested carrier constant in full detail.
	GetCarrierConstant(ctx context.Context, in *GetCarrierConstantRequest, opts ...grpc.CallOption) (*resources.CarrierConstant, error)
}

type carrierConstantServiceClient struct {
	cc grpc.ClientConnInterface
}

func NewCarrierConstantServiceClient(cc grpc.ClientConnInterface) CarrierConstantServiceClient {
	return &carrierConstantServiceClient{cc}
}

func (c *carrierConstantServiceClient) GetCarrierConstant(ctx context.Context, in *GetCarrierConstantRequest, opts ...grpc.CallOption) (*resources.CarrierConstant, error) {
	out := new(resources.CarrierConstant)
	err := c.cc.Invoke(ctx, "/google.ads.googleads.v3.services.CarrierConstantService/GetCarrierConstant", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

// CarrierConstantServiceServer is the server API for CarrierConstantService service.
type CarrierConstantServiceServer interface {
	// Returns the requested carrier constant in full detail.
	GetCarrierConstant(context.Context, *GetCarrierConstantRequest) (*resources.CarrierConstant, error)
}

// UnimplementedCarrierConstantServiceServer can be embedded to have forward compatible implementations.
type UnimplementedCarrierConstantServiceServer struct {
}

func (*UnimplementedCarrierConstantServiceServer) GetCarrierConstant(ctx context.Context, req *GetCarrierConstantRequest) (*resources.CarrierConstant, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetCarrierConstant not implemented")
}

func RegisterCarrierConstantServiceServer(s *grpc.Server, srv CarrierConstantServiceServer) {
	s.RegisterService(&_CarrierConstantService_serviceDesc, srv)
}

func _CarrierConstantService_GetCarrierConstant_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(GetCarrierConstantRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(CarrierConstantServiceServer).GetCarrierConstant(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/google.ads.googleads.v3.services.CarrierConstantService/GetCarrierConstant",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(CarrierConstantServiceServer).GetCarrierConstant(ctx, req.(*GetCarrierConstantRequest))
	}
	return interceptor(ctx, in, info, handler)
}

var _CarrierConstantService_serviceDesc = grpc.ServiceDesc{
	ServiceName: "google.ads.googleads.v3.services.CarrierConstantService",
	HandlerType: (*CarrierConstantServiceServer)(nil),
	Methods: []grpc.MethodDesc{
		{
			MethodName: "GetCarrierConstant",
			Handler:    _CarrierConstantService_GetCarrierConstant_Handler,
		},
	},
	Streams:  []grpc.StreamDesc{},
	Metadata: "google/ads/googleads/v3/services/carrier_constant_service.proto",
}
