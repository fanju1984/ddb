// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/ads/googleads/v3/services/expanded_landing_page_view_service.proto

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

// Request message for
// [ExpandedLandingPageViewService.GetExpandedLandingPageView][google.ads.googleads.v3.services.ExpandedLandingPageViewService.GetExpandedLandingPageView].
type GetExpandedLandingPageViewRequest struct {
	// Required. The resource name of the expanded landing page view to fetch.
	ResourceName         string   `protobuf:"bytes,1,opt,name=resource_name,json=resourceName,proto3" json:"resource_name,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *GetExpandedLandingPageViewRequest) Reset()         { *m = GetExpandedLandingPageViewRequest{} }
func (m *GetExpandedLandingPageViewRequest) String() string { return proto.CompactTextString(m) }
func (*GetExpandedLandingPageViewRequest) ProtoMessage()    {}
func (*GetExpandedLandingPageViewRequest) Descriptor() ([]byte, []int) {
	return fileDescriptor_50015c0516433256, []int{0}
}

func (m *GetExpandedLandingPageViewRequest) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_GetExpandedLandingPageViewRequest.Unmarshal(m, b)
}
func (m *GetExpandedLandingPageViewRequest) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_GetExpandedLandingPageViewRequest.Marshal(b, m, deterministic)
}
func (m *GetExpandedLandingPageViewRequest) XXX_Merge(src proto.Message) {
	xxx_messageInfo_GetExpandedLandingPageViewRequest.Merge(m, src)
}
func (m *GetExpandedLandingPageViewRequest) XXX_Size() int {
	return xxx_messageInfo_GetExpandedLandingPageViewRequest.Size(m)
}
func (m *GetExpandedLandingPageViewRequest) XXX_DiscardUnknown() {
	xxx_messageInfo_GetExpandedLandingPageViewRequest.DiscardUnknown(m)
}

var xxx_messageInfo_GetExpandedLandingPageViewRequest proto.InternalMessageInfo

func (m *GetExpandedLandingPageViewRequest) GetResourceName() string {
	if m != nil {
		return m.ResourceName
	}
	return ""
}

func init() {
	proto.RegisterType((*GetExpandedLandingPageViewRequest)(nil), "google.ads.googleads.v3.services.GetExpandedLandingPageViewRequest")
}

func init() {
	proto.RegisterFile("google/ads/googleads/v3/services/expanded_landing_page_view_service.proto", fileDescriptor_50015c0516433256)
}

var fileDescriptor_50015c0516433256 = []byte{
	// 448 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0x84, 0x93, 0xb1, 0x6f, 0xd4, 0x30,
	0x14, 0xc6, 0x95, 0x20, 0x21, 0x11, 0xc1, 0x92, 0x85, 0x12, 0x10, 0x1c, 0xa5, 0x03, 0xea, 0x60,
	0xa3, 0x66, 0x41, 0x06, 0x06, 0x1f, 0x42, 0x07, 0x12, 0x42, 0xa7, 0x22, 0xdd, 0x80, 0x0e, 0x45,
	0x6e, 0xfc, 0x30, 0x96, 0x12, 0x3b, 0xc4, 0xb9, 0x14, 0x09, 0xb1, 0x74, 0x64, 0xe5, 0x3f, 0x60,
	0x64, 0xe3, 0xdf, 0xe8, 0xca, 0xc6, 0xc4, 0xc0, 0xc4, 0xc6, 0xca, 0x84, 0x52, 0xdb, 0xe9, 0x15,
	0xc9, 0xbd, 0xed, 0x53, 0xbe, 0xcf, 0xbf, 0xf7, 0xfc, 0xfc, 0x92, 0x3c, 0x13, 0x5a, 0x8b, 0x0a,
	0x30, 0xe3, 0x06, 0x5b, 0x39, 0xa8, 0x3e, 0xc7, 0x06, 0xda, 0x5e, 0x96, 0x60, 0x30, 0xbc, 0x6f,
	0x98, 0xe2, 0xc0, 0x8b, 0x8a, 0x29, 0x2e, 0x95, 0x28, 0x1a, 0x26, 0xa0, 0xe8, 0x25, 0x1c, 0x16,
	0x2e, 0x83, 0x9a, 0x56, 0x77, 0x3a, 0x9d, 0xd8, 0xf3, 0x88, 0x71, 0x83, 0x46, 0x14, 0xea, 0x73,
	0xe4, 0x51, 0xd9, 0x34, 0x54, 0xac, 0x05, 0xa3, 0x57, 0xed, 0xf9, 0xd5, 0x6c, 0x95, 0xec, 0x86,
	0x67, 0x34, 0x12, 0x33, 0xa5, 0x74, 0xc7, 0x3a, 0xa9, 0x95, 0x71, 0xee, 0xd5, 0x35, 0xb7, 0xac,
	0x24, 0xa8, 0xce, 0x19, 0xb7, 0xd6, 0x8c, 0x37, 0x12, 0x2a, 0x5e, 0x1c, 0xc0, 0x5b, 0xd6, 0x4b,
	0xdd, 0xba, 0xc0, 0xb5, 0xb5, 0x80, 0x6f, 0xc7, 0x5a, 0xdb, 0x47, 0x51, 0x72, 0x7b, 0x06, 0xdd,
	0x13, 0xd7, 0xda, 0x73, 0xdb, 0xd9, 0x9c, 0x09, 0x58, 0x48, 0x38, 0xdc, 0x87, 0x77, 0x2b, 0x30,
	0x5d, 0xfa, 0x3a, 0xb9, 0xe2, 0xcf, 0x15, 0x8a, 0xd5, 0xb0, 0x15, 0x4d, 0xa2, 0xbb, 0x97, 0xa6,
	0xf7, 0x7f, 0xd2, 0xf8, 0x2f, 0xdd, 0x4b, 0xee, 0x9d, 0x8e, 0xc4, 0xa9, 0x46, 0x1a, 0x54, 0xea,
	0x1a, 0x87, 0xb8, 0x97, 0x3d, 0xee, 0x05, 0xab, 0x61, 0xef, 0x5b, 0x9c, 0xdc, 0x0c, 0x24, 0x5f,
	0xda, 0xf9, 0xa6, 0x7f, 0xa2, 0x24, 0x0b, 0xf7, 0x99, 0x3e, 0x46, 0x9b, 0x1e, 0x08, 0x6d, 0xbc,
	0x65, 0x46, 0x82, 0x90, 0xf1, 0x0d, 0x51, 0x00, 0xb1, 0xbd, 0xff, 0x83, 0x9e, 0x1d, 0xd1, 0xd1,
	0xf7, 0x5f, 0x9f, 0xe3, 0x87, 0x29, 0x19, 0x56, 0xe0, 0xc3, 0x19, 0xe7, 0x51, 0xb9, 0x32, 0x9d,
	0xae, 0xa1, 0x35, 0x78, 0x77, 0xdc, 0x89, 0xff, 0x78, 0x06, 0xef, 0x7e, 0xcc, 0xae, 0x1f, 0xd3,
	0xad, 0xd0, 0x64, 0xa7, 0x9f, 0xe2, 0x64, 0xa7, 0xd4, 0xf5, 0xc6, 0x7b, 0x4f, 0xef, 0x9c, 0x3f,
	0xd9, 0xf9, 0xb0, 0x06, 0xf3, 0xe8, 0xd5, 0x53, 0x07, 0x12, 0xba, 0x62, 0x4a, 0x20, 0xdd, 0x0a,
	0x2c, 0x40, 0x9d, 0x2c, 0x09, 0x3e, 0x2d, 0x1d, 0xfe, 0x97, 0x1e, 0x78, 0xf1, 0x25, 0xbe, 0x30,
	0xa3, 0xf4, 0x6b, 0x3c, 0x99, 0x59, 0x20, 0xe5, 0x06, 0x59, 0x39, 0xa8, 0x45, 0x8e, 0x5c, 0x61,
	0x73, 0xec, 0x23, 0x4b, 0xca, 0xcd, 0x72, 0x8c, 0x2c, 0x17, 0xf9, 0xd2, 0x47, 0x7e, 0xc7, 0x3b,
	0xf6, 0x3b, 0x21, 0x94, 0x1b, 0x42, 0xc6, 0x10, 0x21, 0x8b, 0x9c, 0x10, 0x1f, 0x3b, 0xb8, 0x78,
	0xd2, 0x67, 0xfe, 0x2f, 0x00, 0x00, 0xff, 0xff, 0x52, 0x94, 0x51, 0x63, 0xf2, 0x03, 0x00, 0x00,
}

// Reference imports to suppress errors if they are not otherwise used.
var _ context.Context
var _ grpc.ClientConnInterface

// This is a compile-time assertion to ensure that this generated file
// is compatible with the grpc package it is being compiled against.
const _ = grpc.SupportPackageIsVersion6

// ExpandedLandingPageViewServiceClient is the client API for ExpandedLandingPageViewService service.
//
// For semantics around ctx use and closing/ending streaming RPCs, please refer to https://godoc.org/google.golang.org/grpc#ClientConn.NewStream.
type ExpandedLandingPageViewServiceClient interface {
	// Returns the requested expanded landing page view in full detail.
	GetExpandedLandingPageView(ctx context.Context, in *GetExpandedLandingPageViewRequest, opts ...grpc.CallOption) (*resources.ExpandedLandingPageView, error)
}

type expandedLandingPageViewServiceClient struct {
	cc grpc.ClientConnInterface
}

func NewExpandedLandingPageViewServiceClient(cc grpc.ClientConnInterface) ExpandedLandingPageViewServiceClient {
	return &expandedLandingPageViewServiceClient{cc}
}

func (c *expandedLandingPageViewServiceClient) GetExpandedLandingPageView(ctx context.Context, in *GetExpandedLandingPageViewRequest, opts ...grpc.CallOption) (*resources.ExpandedLandingPageView, error) {
	out := new(resources.ExpandedLandingPageView)
	err := c.cc.Invoke(ctx, "/google.ads.googleads.v3.services.ExpandedLandingPageViewService/GetExpandedLandingPageView", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

// ExpandedLandingPageViewServiceServer is the server API for ExpandedLandingPageViewService service.
type ExpandedLandingPageViewServiceServer interface {
	// Returns the requested expanded landing page view in full detail.
	GetExpandedLandingPageView(context.Context, *GetExpandedLandingPageViewRequest) (*resources.ExpandedLandingPageView, error)
}

// UnimplementedExpandedLandingPageViewServiceServer can be embedded to have forward compatible implementations.
type UnimplementedExpandedLandingPageViewServiceServer struct {
}

func (*UnimplementedExpandedLandingPageViewServiceServer) GetExpandedLandingPageView(ctx context.Context, req *GetExpandedLandingPageViewRequest) (*resources.ExpandedLandingPageView, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetExpandedLandingPageView not implemented")
}

func RegisterExpandedLandingPageViewServiceServer(s *grpc.Server, srv ExpandedLandingPageViewServiceServer) {
	s.RegisterService(&_ExpandedLandingPageViewService_serviceDesc, srv)
}

func _ExpandedLandingPageViewService_GetExpandedLandingPageView_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(GetExpandedLandingPageViewRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(ExpandedLandingPageViewServiceServer).GetExpandedLandingPageView(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/google.ads.googleads.v3.services.ExpandedLandingPageViewService/GetExpandedLandingPageView",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(ExpandedLandingPageViewServiceServer).GetExpandedLandingPageView(ctx, req.(*GetExpandedLandingPageViewRequest))
	}
	return interceptor(ctx, in, info, handler)
}

var _ExpandedLandingPageViewService_serviceDesc = grpc.ServiceDesc{
	ServiceName: "google.ads.googleads.v3.services.ExpandedLandingPageViewService",
	HandlerType: (*ExpandedLandingPageViewServiceServer)(nil),
	Methods: []grpc.MethodDesc{
		{
			MethodName: "GetExpandedLandingPageView",
			Handler:    _ExpandedLandingPageViewService_GetExpandedLandingPageView_Handler,
		},
	},
	Streams:  []grpc.StreamDesc{},
	Metadata: "google/ads/googleads/v3/services/expanded_landing_page_view_service.proto",
}