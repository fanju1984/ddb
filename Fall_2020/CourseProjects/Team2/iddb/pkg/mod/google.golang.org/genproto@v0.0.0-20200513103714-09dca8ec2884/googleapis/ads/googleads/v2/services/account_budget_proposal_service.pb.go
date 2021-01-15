// Code generated by protoc-gen-go. DO NOT EDIT.
// source: google/ads/googleads/v2/services/account_budget_proposal_service.proto

package services

import (
	context "context"
	fmt "fmt"
	math "math"

	proto "github.com/golang/protobuf/proto"
	resources "google.golang.org/genproto/googleapis/ads/googleads/v2/resources"
	_ "google.golang.org/genproto/googleapis/api/annotations"
	field_mask "google.golang.org/genproto/protobuf/field_mask"
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
// [AccountBudgetProposalService.GetAccountBudgetProposal][google.ads.googleads.v2.services.AccountBudgetProposalService.GetAccountBudgetProposal].
type GetAccountBudgetProposalRequest struct {
	// Required. The resource name of the account-level budget proposal to fetch.
	ResourceName         string   `protobuf:"bytes,1,opt,name=resource_name,json=resourceName,proto3" json:"resource_name,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *GetAccountBudgetProposalRequest) Reset()         { *m = GetAccountBudgetProposalRequest{} }
func (m *GetAccountBudgetProposalRequest) String() string { return proto.CompactTextString(m) }
func (*GetAccountBudgetProposalRequest) ProtoMessage()    {}
func (*GetAccountBudgetProposalRequest) Descriptor() ([]byte, []int) {
	return fileDescriptor_f1f46d07b5a8af03, []int{0}
}

func (m *GetAccountBudgetProposalRequest) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_GetAccountBudgetProposalRequest.Unmarshal(m, b)
}
func (m *GetAccountBudgetProposalRequest) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_GetAccountBudgetProposalRequest.Marshal(b, m, deterministic)
}
func (m *GetAccountBudgetProposalRequest) XXX_Merge(src proto.Message) {
	xxx_messageInfo_GetAccountBudgetProposalRequest.Merge(m, src)
}
func (m *GetAccountBudgetProposalRequest) XXX_Size() int {
	return xxx_messageInfo_GetAccountBudgetProposalRequest.Size(m)
}
func (m *GetAccountBudgetProposalRequest) XXX_DiscardUnknown() {
	xxx_messageInfo_GetAccountBudgetProposalRequest.DiscardUnknown(m)
}

var xxx_messageInfo_GetAccountBudgetProposalRequest proto.InternalMessageInfo

func (m *GetAccountBudgetProposalRequest) GetResourceName() string {
	if m != nil {
		return m.ResourceName
	}
	return ""
}

// Request message for
// [AccountBudgetProposalService.MutateAccountBudgetProposal][google.ads.googleads.v2.services.AccountBudgetProposalService.MutateAccountBudgetProposal].
type MutateAccountBudgetProposalRequest struct {
	// Required. The ID of the customer.
	CustomerId string `protobuf:"bytes,1,opt,name=customer_id,json=customerId,proto3" json:"customer_id,omitempty"`
	// Required. The operation to perform on an individual account-level budget proposal.
	Operation *AccountBudgetProposalOperation `protobuf:"bytes,2,opt,name=operation,proto3" json:"operation,omitempty"`
	// If true, the request is validated but not executed. Only errors are
	// returned, not results.
	ValidateOnly         bool     `protobuf:"varint,3,opt,name=validate_only,json=validateOnly,proto3" json:"validate_only,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *MutateAccountBudgetProposalRequest) Reset()         { *m = MutateAccountBudgetProposalRequest{} }
func (m *MutateAccountBudgetProposalRequest) String() string { return proto.CompactTextString(m) }
func (*MutateAccountBudgetProposalRequest) ProtoMessage()    {}
func (*MutateAccountBudgetProposalRequest) Descriptor() ([]byte, []int) {
	return fileDescriptor_f1f46d07b5a8af03, []int{1}
}

func (m *MutateAccountBudgetProposalRequest) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_MutateAccountBudgetProposalRequest.Unmarshal(m, b)
}
func (m *MutateAccountBudgetProposalRequest) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_MutateAccountBudgetProposalRequest.Marshal(b, m, deterministic)
}
func (m *MutateAccountBudgetProposalRequest) XXX_Merge(src proto.Message) {
	xxx_messageInfo_MutateAccountBudgetProposalRequest.Merge(m, src)
}
func (m *MutateAccountBudgetProposalRequest) XXX_Size() int {
	return xxx_messageInfo_MutateAccountBudgetProposalRequest.Size(m)
}
func (m *MutateAccountBudgetProposalRequest) XXX_DiscardUnknown() {
	xxx_messageInfo_MutateAccountBudgetProposalRequest.DiscardUnknown(m)
}

var xxx_messageInfo_MutateAccountBudgetProposalRequest proto.InternalMessageInfo

func (m *MutateAccountBudgetProposalRequest) GetCustomerId() string {
	if m != nil {
		return m.CustomerId
	}
	return ""
}

func (m *MutateAccountBudgetProposalRequest) GetOperation() *AccountBudgetProposalOperation {
	if m != nil {
		return m.Operation
	}
	return nil
}

func (m *MutateAccountBudgetProposalRequest) GetValidateOnly() bool {
	if m != nil {
		return m.ValidateOnly
	}
	return false
}

// A single operation to propose the creation of a new account-level budget or
// edit/end/remove an existing one.
type AccountBudgetProposalOperation struct {
	// FieldMask that determines which budget fields are modified.  While budgets
	// may be modified, proposals that propose such modifications are final.
	// Therefore, update operations are not supported for proposals.
	//
	// Proposals that modify budgets have the 'update' proposal type.  Specifying
	// a mask for any other proposal type is considered an error.
	UpdateMask *field_mask.FieldMask `protobuf:"bytes,3,opt,name=update_mask,json=updateMask,proto3" json:"update_mask,omitempty"`
	// The mutate operation.
	//
	// Types that are valid to be assigned to Operation:
	//	*AccountBudgetProposalOperation_Create
	//	*AccountBudgetProposalOperation_Remove
	Operation            isAccountBudgetProposalOperation_Operation `protobuf_oneof:"operation"`
	XXX_NoUnkeyedLiteral struct{}                                   `json:"-"`
	XXX_unrecognized     []byte                                     `json:"-"`
	XXX_sizecache        int32                                      `json:"-"`
}

func (m *AccountBudgetProposalOperation) Reset()         { *m = AccountBudgetProposalOperation{} }
func (m *AccountBudgetProposalOperation) String() string { return proto.CompactTextString(m) }
func (*AccountBudgetProposalOperation) ProtoMessage()    {}
func (*AccountBudgetProposalOperation) Descriptor() ([]byte, []int) {
	return fileDescriptor_f1f46d07b5a8af03, []int{2}
}

func (m *AccountBudgetProposalOperation) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_AccountBudgetProposalOperation.Unmarshal(m, b)
}
func (m *AccountBudgetProposalOperation) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_AccountBudgetProposalOperation.Marshal(b, m, deterministic)
}
func (m *AccountBudgetProposalOperation) XXX_Merge(src proto.Message) {
	xxx_messageInfo_AccountBudgetProposalOperation.Merge(m, src)
}
func (m *AccountBudgetProposalOperation) XXX_Size() int {
	return xxx_messageInfo_AccountBudgetProposalOperation.Size(m)
}
func (m *AccountBudgetProposalOperation) XXX_DiscardUnknown() {
	xxx_messageInfo_AccountBudgetProposalOperation.DiscardUnknown(m)
}

var xxx_messageInfo_AccountBudgetProposalOperation proto.InternalMessageInfo

func (m *AccountBudgetProposalOperation) GetUpdateMask() *field_mask.FieldMask {
	if m != nil {
		return m.UpdateMask
	}
	return nil
}

type isAccountBudgetProposalOperation_Operation interface {
	isAccountBudgetProposalOperation_Operation()
}

type AccountBudgetProposalOperation_Create struct {
	Create *resources.AccountBudgetProposal `protobuf:"bytes,2,opt,name=create,proto3,oneof"`
}

type AccountBudgetProposalOperation_Remove struct {
	Remove string `protobuf:"bytes,1,opt,name=remove,proto3,oneof"`
}

func (*AccountBudgetProposalOperation_Create) isAccountBudgetProposalOperation_Operation() {}

func (*AccountBudgetProposalOperation_Remove) isAccountBudgetProposalOperation_Operation() {}

func (m *AccountBudgetProposalOperation) GetOperation() isAccountBudgetProposalOperation_Operation {
	if m != nil {
		return m.Operation
	}
	return nil
}

func (m *AccountBudgetProposalOperation) GetCreate() *resources.AccountBudgetProposal {
	if x, ok := m.GetOperation().(*AccountBudgetProposalOperation_Create); ok {
		return x.Create
	}
	return nil
}

func (m *AccountBudgetProposalOperation) GetRemove() string {
	if x, ok := m.GetOperation().(*AccountBudgetProposalOperation_Remove); ok {
		return x.Remove
	}
	return ""
}

// XXX_OneofWrappers is for the internal use of the proto package.
func (*AccountBudgetProposalOperation) XXX_OneofWrappers() []interface{} {
	return []interface{}{
		(*AccountBudgetProposalOperation_Create)(nil),
		(*AccountBudgetProposalOperation_Remove)(nil),
	}
}

// Response message for account-level budget mutate operations.
type MutateAccountBudgetProposalResponse struct {
	// The result of the mutate.
	Result               *MutateAccountBudgetProposalResult `protobuf:"bytes,2,opt,name=result,proto3" json:"result,omitempty"`
	XXX_NoUnkeyedLiteral struct{}                           `json:"-"`
	XXX_unrecognized     []byte                             `json:"-"`
	XXX_sizecache        int32                              `json:"-"`
}

func (m *MutateAccountBudgetProposalResponse) Reset()         { *m = MutateAccountBudgetProposalResponse{} }
func (m *MutateAccountBudgetProposalResponse) String() string { return proto.CompactTextString(m) }
func (*MutateAccountBudgetProposalResponse) ProtoMessage()    {}
func (*MutateAccountBudgetProposalResponse) Descriptor() ([]byte, []int) {
	return fileDescriptor_f1f46d07b5a8af03, []int{3}
}

func (m *MutateAccountBudgetProposalResponse) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_MutateAccountBudgetProposalResponse.Unmarshal(m, b)
}
func (m *MutateAccountBudgetProposalResponse) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_MutateAccountBudgetProposalResponse.Marshal(b, m, deterministic)
}
func (m *MutateAccountBudgetProposalResponse) XXX_Merge(src proto.Message) {
	xxx_messageInfo_MutateAccountBudgetProposalResponse.Merge(m, src)
}
func (m *MutateAccountBudgetProposalResponse) XXX_Size() int {
	return xxx_messageInfo_MutateAccountBudgetProposalResponse.Size(m)
}
func (m *MutateAccountBudgetProposalResponse) XXX_DiscardUnknown() {
	xxx_messageInfo_MutateAccountBudgetProposalResponse.DiscardUnknown(m)
}

var xxx_messageInfo_MutateAccountBudgetProposalResponse proto.InternalMessageInfo

func (m *MutateAccountBudgetProposalResponse) GetResult() *MutateAccountBudgetProposalResult {
	if m != nil {
		return m.Result
	}
	return nil
}

// The result for the account budget proposal mutate.
type MutateAccountBudgetProposalResult struct {
	// Returned for successful operations.
	ResourceName         string   `protobuf:"bytes,1,opt,name=resource_name,json=resourceName,proto3" json:"resource_name,omitempty"`
	XXX_NoUnkeyedLiteral struct{} `json:"-"`
	XXX_unrecognized     []byte   `json:"-"`
	XXX_sizecache        int32    `json:"-"`
}

func (m *MutateAccountBudgetProposalResult) Reset()         { *m = MutateAccountBudgetProposalResult{} }
func (m *MutateAccountBudgetProposalResult) String() string { return proto.CompactTextString(m) }
func (*MutateAccountBudgetProposalResult) ProtoMessage()    {}
func (*MutateAccountBudgetProposalResult) Descriptor() ([]byte, []int) {
	return fileDescriptor_f1f46d07b5a8af03, []int{4}
}

func (m *MutateAccountBudgetProposalResult) XXX_Unmarshal(b []byte) error {
	return xxx_messageInfo_MutateAccountBudgetProposalResult.Unmarshal(m, b)
}
func (m *MutateAccountBudgetProposalResult) XXX_Marshal(b []byte, deterministic bool) ([]byte, error) {
	return xxx_messageInfo_MutateAccountBudgetProposalResult.Marshal(b, m, deterministic)
}
func (m *MutateAccountBudgetProposalResult) XXX_Merge(src proto.Message) {
	xxx_messageInfo_MutateAccountBudgetProposalResult.Merge(m, src)
}
func (m *MutateAccountBudgetProposalResult) XXX_Size() int {
	return xxx_messageInfo_MutateAccountBudgetProposalResult.Size(m)
}
func (m *MutateAccountBudgetProposalResult) XXX_DiscardUnknown() {
	xxx_messageInfo_MutateAccountBudgetProposalResult.DiscardUnknown(m)
}

var xxx_messageInfo_MutateAccountBudgetProposalResult proto.InternalMessageInfo

func (m *MutateAccountBudgetProposalResult) GetResourceName() string {
	if m != nil {
		return m.ResourceName
	}
	return ""
}

func init() {
	proto.RegisterType((*GetAccountBudgetProposalRequest)(nil), "google.ads.googleads.v2.services.GetAccountBudgetProposalRequest")
	proto.RegisterType((*MutateAccountBudgetProposalRequest)(nil), "google.ads.googleads.v2.services.MutateAccountBudgetProposalRequest")
	proto.RegisterType((*AccountBudgetProposalOperation)(nil), "google.ads.googleads.v2.services.AccountBudgetProposalOperation")
	proto.RegisterType((*MutateAccountBudgetProposalResponse)(nil), "google.ads.googleads.v2.services.MutateAccountBudgetProposalResponse")
	proto.RegisterType((*MutateAccountBudgetProposalResult)(nil), "google.ads.googleads.v2.services.MutateAccountBudgetProposalResult")
}

func init() {
	proto.RegisterFile("google/ads/googleads/v2/services/account_budget_proposal_service.proto", fileDescriptor_f1f46d07b5a8af03)
}

var fileDescriptor_f1f46d07b5a8af03 = []byte{
	// 710 bytes of a gzipped FileDescriptorProto
	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xff, 0xa4, 0x95, 0x3d, 0x6f, 0xd3, 0x40,
	0x18, 0xc7, 0xb1, 0x8b, 0x2a, 0x7a, 0x69, 0x17, 0x4b, 0x88, 0x90, 0x56, 0x34, 0x75, 0x3b, 0x54,
	0x11, 0xb2, 0x91, 0x91, 0x50, 0xe5, 0xaa, 0xa2, 0x0e, 0xd0, 0x96, 0xa1, 0x34, 0x32, 0x52, 0x07,
	0x1a, 0x61, 0x5d, 0xec, 0xab, 0xb1, 0x6a, 0xfb, 0x8c, 0xef, 0x1c, 0xa9, 0xaa, 0xca, 0xd0, 0x09,
	0xb1, 0xf2, 0x0d, 0x18, 0x99, 0xf9, 0x14, 0x5d, 0x61, 0x40, 0x9d, 0x3a, 0xb0, 0xc0, 0x47, 0x60,
	0x42, 0xf6, 0xdd, 0x25, 0x8e, 0x94, 0x38, 0x12, 0xdd, 0x1e, 0xdf, 0xf3, 0xcf, 0xef, 0x79, 0xbd,
	0x0b, 0xd8, 0xf1, 0x31, 0xf6, 0x43, 0xa4, 0x43, 0x8f, 0xe8, 0xcc, 0xcc, 0xad, 0xbe, 0xa1, 0x13,
	0x94, 0xf6, 0x03, 0x17, 0x11, 0x1d, 0xba, 0x2e, 0xce, 0x62, 0xea, 0xf4, 0x32, 0xcf, 0x47, 0xd4,
	0x49, 0x52, 0x9c, 0x60, 0x02, 0x43, 0x87, 0x0b, 0xb4, 0x24, 0xc5, 0x14, 0x2b, 0x4d, 0xf6, 0x63,
	0x0d, 0x7a, 0x44, 0x1b, 0x70, 0xb4, 0xbe, 0xa1, 0x09, 0x4e, 0xe3, 0xe9, 0xa4, 0x48, 0x29, 0x22,
	0x38, 0x4b, 0x2b, 0x42, 0xb1, 0x10, 0x8d, 0x25, 0x01, 0x48, 0x02, 0x1d, 0xc6, 0x31, 0xa6, 0x90,
	0x06, 0x38, 0x26, 0xdc, 0x7b, 0xaf, 0xe4, 0x75, 0xc3, 0x00, 0xc5, 0x94, 0x3b, 0x96, 0x4b, 0x8e,
	0xe3, 0x00, 0x85, 0x9e, 0xd3, 0x43, 0xef, 0x60, 0x3f, 0xc0, 0x29, 0x17, 0xdc, 0x2f, 0x09, 0x44,
	0x2e, 0xdc, 0xc5, 0xab, 0xd2, 0x8b, 0xaf, 0x5e, 0x76, 0xcc, 0x01, 0x11, 0x24, 0x27, 0x4c, 0xa1,
	0x7e, 0x00, 0xcb, 0xbb, 0x88, 0x5a, 0x2c, 0xf1, 0x76, 0x91, 0x77, 0x87, 0xa7, 0x6d, 0xa3, 0xf7,
	0x19, 0x22, 0x54, 0x39, 0x02, 0x0b, 0x02, 0xeb, 0xc4, 0x30, 0x42, 0x75, 0xa9, 0x29, 0xad, 0xcf,
	0xb5, 0x9f, 0x5c, 0x5b, 0xf2, 0x5f, 0xeb, 0x11, 0x28, 0xb5, 0x8b, 0x5b, 0x49, 0x40, 0x34, 0x17,
	0x47, 0xfa, 0x78, 0xea, 0xbc, 0x80, 0xbd, 0x82, 0x11, 0x52, 0x7f, 0x48, 0x40, 0xdd, 0xcf, 0x28,
	0xa4, 0xa8, 0x32, 0x87, 0x35, 0x50, 0x73, 0x33, 0x42, 0x71, 0x84, 0x52, 0x27, 0xf0, 0x78, 0x06,
	0x33, 0xd7, 0x96, 0x6c, 0x03, 0x71, 0xfe, 0xd2, 0x53, 0x5c, 0x30, 0x87, 0x13, 0x94, 0x16, 0x7d,
	0xad, 0xcb, 0x4d, 0x69, 0xbd, 0x66, 0x6c, 0x6b, 0xd3, 0x06, 0xab, 0x8d, 0x0d, 0x7c, 0x20, 0x38,
	0x2c, 0xca, 0x90, 0xab, 0xac, 0x82, 0x85, 0x3e, 0x0c, 0x03, 0x0f, 0x52, 0xe4, 0xe0, 0x38, 0x3c,
	0xad, 0xcf, 0x34, 0xa5, 0xf5, 0x3b, 0xf6, 0xbc, 0x38, 0x3c, 0x88, 0xc3, 0x53, 0xf5, 0xa7, 0x04,
	0x1e, 0x54, 0x73, 0x95, 0x4d, 0x50, 0xcb, 0x92, 0x82, 0x92, 0x8f, 0xa3, 0xa0, 0xd4, 0x8c, 0x86,
	0x48, 0x57, 0x4c, 0x4c, 0xdb, 0xc9, 0x27, 0xb6, 0x0f, 0xc9, 0x89, 0x0d, 0x98, 0x3c, 0xb7, 0x15,
	0x1b, 0xcc, 0xba, 0x29, 0x82, 0x14, 0xf1, 0x32, 0x37, 0x26, 0x96, 0x39, 0xd8, 0xce, 0xf1, 0x75,
	0xee, 0xdd, 0xb2, 0x39, 0x49, 0xa9, 0x83, 0xd9, 0x14, 0x45, 0xb8, 0xcf, 0x07, 0x9c, 0x7b, 0xd8,
	0x77, 0xbb, 0x56, 0xea, 0xab, 0x7a, 0x21, 0x81, 0xd5, 0xca, 0x89, 0x91, 0x04, 0xc7, 0x04, 0x29,
	0x47, 0x39, 0x8e, 0x64, 0x21, 0xe5, 0x29, 0x3e, 0x9b, 0x3e, 0x89, 0x6a, 0x6c, 0x16, 0x52, 0x9b,
	0x23, 0xd5, 0x3d, 0xb0, 0x32, 0x55, 0x9c, 0x4f, 0x6a, 0xcc, 0xe2, 0x8e, 0x2e, 0xa0, 0xf1, 0xed,
	0x36, 0x58, 0x1a, 0x0b, 0x79, 0xcd, 0xb2, 0x52, 0x7e, 0x4b, 0xa0, 0x3e, 0xe9, 0x8a, 0x28, 0xd6,
	0xf4, 0xa2, 0xa6, 0x5c, 0xaf, 0xc6, 0x7f, 0x8f, 0x4e, 0xed, 0x5c, 0x59, 0xa3, 0x05, 0x5e, 0x7c,
	0xff, 0xf5, 0x59, 0x36, 0x95, 0x8d, 0xfc, 0x55, 0x3a, 0x1b, 0xf1, 0x6c, 0x89, 0x5b, 0x42, 0xf4,
	0x96, 0x78, 0xa6, 0x46, 0x69, 0x44, 0x6f, 0x9d, 0x2b, 0x9f, 0x64, 0xb0, 0x58, 0xd1, 0x57, 0xe5,
	0xf9, 0x0d, 0x67, 0xc8, 0x2a, 0x7e, 0x71, 0xd3, 0x4d, 0x28, 0x16, 0x4c, 0x7d, 0x7b, 0x65, 0xdd,
	0x2d, 0x3d, 0x0a, 0x0f, 0x07, 0x2b, 0x5a, 0xb4, 0x61, 0x5b, 0xdd, 0xcc, 0xdb, 0x30, 0xac, 0xfb,
	0xac, 0xa4, 0xdd, 0x6a, 0x9d, 0x4f, 0xe8, 0x82, 0x19, 0x15, 0x91, 0x4d, 0xa9, 0xd5, 0x58, 0xbc,
	0xb4, 0xea, 0x93, 0xde, 0xb6, 0xf6, 0x47, 0x19, 0xac, 0xb9, 0x38, 0x9a, 0x5a, 0x49, 0x7b, 0xa5,
	0x6a, 0xb9, 0x3a, 0xf9, 0x2d, 0xef, 0x48, 0x6f, 0xf6, 0x38, 0xc6, 0xc7, 0x21, 0x8c, 0x7d, 0x0d,
	0xa7, 0xbe, 0xee, 0xa3, 0xb8, 0x78, 0x03, 0xf4, 0x61, 0xe0, 0xc9, 0x7f, 0x72, 0x9b, 0xc2, 0xf8,
	0x22, 0xcf, 0xec, 0x5a, 0xd6, 0x57, 0xb9, 0xb9, 0xcb, 0x80, 0x96, 0x47, 0x34, 0x66, 0xe6, 0xd6,
	0xa1, 0xa1, 0xf1, 0xc0, 0xe4, 0x52, 0x48, 0xba, 0x96, 0x47, 0xba, 0x03, 0x49, 0xf7, 0xd0, 0xe8,
	0x0a, 0xc9, 0x1f, 0x79, 0x8d, 0x9d, 0x9b, 0xa6, 0xe5, 0x11, 0xd3, 0x1c, 0x88, 0x4c, 0xf3, 0xd0,
	0x30, 0x4d, 0x21, 0xeb, 0xcd, 0x16, 0x79, 0x3e, 0xfe, 0x17, 0x00, 0x00, 0xff, 0xff, 0x69, 0x7c,
	0x1e, 0x73, 0x8b, 0x07, 0x00, 0x00,
}

// Reference imports to suppress errors if they are not otherwise used.
var _ context.Context
var _ grpc.ClientConnInterface

// This is a compile-time assertion to ensure that this generated file
// is compatible with the grpc package it is being compiled against.
const _ = grpc.SupportPackageIsVersion6

// AccountBudgetProposalServiceClient is the client API for AccountBudgetProposalService service.
//
// For semantics around ctx use and closing/ending streaming RPCs, please refer to https://godoc.org/google.golang.org/grpc#ClientConn.NewStream.
type AccountBudgetProposalServiceClient interface {
	// Returns an account-level budget proposal in full detail.
	GetAccountBudgetProposal(ctx context.Context, in *GetAccountBudgetProposalRequest, opts ...grpc.CallOption) (*resources.AccountBudgetProposal, error)
	// Creates, updates, or removes account budget proposals.  Operation statuses
	// are returned.
	MutateAccountBudgetProposal(ctx context.Context, in *MutateAccountBudgetProposalRequest, opts ...grpc.CallOption) (*MutateAccountBudgetProposalResponse, error)
}

type accountBudgetProposalServiceClient struct {
	cc grpc.ClientConnInterface
}

func NewAccountBudgetProposalServiceClient(cc grpc.ClientConnInterface) AccountBudgetProposalServiceClient {
	return &accountBudgetProposalServiceClient{cc}
}

func (c *accountBudgetProposalServiceClient) GetAccountBudgetProposal(ctx context.Context, in *GetAccountBudgetProposalRequest, opts ...grpc.CallOption) (*resources.AccountBudgetProposal, error) {
	out := new(resources.AccountBudgetProposal)
	err := c.cc.Invoke(ctx, "/google.ads.googleads.v2.services.AccountBudgetProposalService/GetAccountBudgetProposal", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

func (c *accountBudgetProposalServiceClient) MutateAccountBudgetProposal(ctx context.Context, in *MutateAccountBudgetProposalRequest, opts ...grpc.CallOption) (*MutateAccountBudgetProposalResponse, error) {
	out := new(MutateAccountBudgetProposalResponse)
	err := c.cc.Invoke(ctx, "/google.ads.googleads.v2.services.AccountBudgetProposalService/MutateAccountBudgetProposal", in, out, opts...)
	if err != nil {
		return nil, err
	}
	return out, nil
}

// AccountBudgetProposalServiceServer is the server API for AccountBudgetProposalService service.
type AccountBudgetProposalServiceServer interface {
	// Returns an account-level budget proposal in full detail.
	GetAccountBudgetProposal(context.Context, *GetAccountBudgetProposalRequest) (*resources.AccountBudgetProposal, error)
	// Creates, updates, or removes account budget proposals.  Operation statuses
	// are returned.
	MutateAccountBudgetProposal(context.Context, *MutateAccountBudgetProposalRequest) (*MutateAccountBudgetProposalResponse, error)
}

// UnimplementedAccountBudgetProposalServiceServer can be embedded to have forward compatible implementations.
type UnimplementedAccountBudgetProposalServiceServer struct {
}

func (*UnimplementedAccountBudgetProposalServiceServer) GetAccountBudgetProposal(ctx context.Context, req *GetAccountBudgetProposalRequest) (*resources.AccountBudgetProposal, error) {
	return nil, status.Errorf(codes.Unimplemented, "method GetAccountBudgetProposal not implemented")
}
func (*UnimplementedAccountBudgetProposalServiceServer) MutateAccountBudgetProposal(ctx context.Context, req *MutateAccountBudgetProposalRequest) (*MutateAccountBudgetProposalResponse, error) {
	return nil, status.Errorf(codes.Unimplemented, "method MutateAccountBudgetProposal not implemented")
}

func RegisterAccountBudgetProposalServiceServer(s *grpc.Server, srv AccountBudgetProposalServiceServer) {
	s.RegisterService(&_AccountBudgetProposalService_serviceDesc, srv)
}

func _AccountBudgetProposalService_GetAccountBudgetProposal_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(GetAccountBudgetProposalRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(AccountBudgetProposalServiceServer).GetAccountBudgetProposal(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/google.ads.googleads.v2.services.AccountBudgetProposalService/GetAccountBudgetProposal",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(AccountBudgetProposalServiceServer).GetAccountBudgetProposal(ctx, req.(*GetAccountBudgetProposalRequest))
	}
	return interceptor(ctx, in, info, handler)
}

func _AccountBudgetProposalService_MutateAccountBudgetProposal_Handler(srv interface{}, ctx context.Context, dec func(interface{}) error, interceptor grpc.UnaryServerInterceptor) (interface{}, error) {
	in := new(MutateAccountBudgetProposalRequest)
	if err := dec(in); err != nil {
		return nil, err
	}
	if interceptor == nil {
		return srv.(AccountBudgetProposalServiceServer).MutateAccountBudgetProposal(ctx, in)
	}
	info := &grpc.UnaryServerInfo{
		Server:     srv,
		FullMethod: "/google.ads.googleads.v2.services.AccountBudgetProposalService/MutateAccountBudgetProposal",
	}
	handler := func(ctx context.Context, req interface{}) (interface{}, error) {
		return srv.(AccountBudgetProposalServiceServer).MutateAccountBudgetProposal(ctx, req.(*MutateAccountBudgetProposalRequest))
	}
	return interceptor(ctx, in, info, handler)
}

var _AccountBudgetProposalService_serviceDesc = grpc.ServiceDesc{
	ServiceName: "google.ads.googleads.v2.services.AccountBudgetProposalService",
	HandlerType: (*AccountBudgetProposalServiceServer)(nil),
	Methods: []grpc.MethodDesc{
		{
			MethodName: "GetAccountBudgetProposal",
			Handler:    _AccountBudgetProposalService_GetAccountBudgetProposal_Handler,
		},
		{
			MethodName: "MutateAccountBudgetProposal",
			Handler:    _AccountBudgetProposalService_MutateAccountBudgetProposal_Handler,
		},
	},
	Streams:  []grpc.StreamDesc{},
	Metadata: "google/ads/googleads/v2/services/account_budget_proposal_service.proto",
}