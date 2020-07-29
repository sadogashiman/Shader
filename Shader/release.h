#pragma once
////////////////////////////////////
// �֐��`���}�N��
////////////////////////////////////

////////////////////////////////////
// �C���^�[�t�F�C�X���
////////////////////////////////////
template <typename T>
inline void safeRelease(T& Ptr) { if (Ptr != nullptr)Ptr->Release(); Ptr = nullptr; }

////////////////////////////////////
// ���������
////////////////////////////////////
#define SAFE_DELETE(x)         {delete x;  x=nullptr;}// 1��
#define SAFE_DELETE_ARRAY(x)   {delete[] x;x=nullptr;}// �z��
#define SAFE_DELETE_DESTROY(x) {x->destroy();delete x;x = nullptr;}