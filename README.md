# C++20 모듈과 IOCP로 완성하는 MMORPG 서버

이 저장소는 Velog 블로그 시리즈 **C++20 & IOCP**를 기반으로 작성되었습니다.  
개발 과정에서 겪었던 고민과 시행착오, 그리고 문제 해결 과정을 공유합니다.

- ModuleServer : C++20 버전으로 만든 서버
- OldServer : C++14 버전으로 만든 서버

---

## 📖 기술 블로그 시리즈 링크

- [기술 블로그 시리즈 전체 보기](https://velog.io/@minseok0406/series/C20-IOCP)

---

## 📂 글 목록

### 1️⃣ [환경설정](https://velog.io/@minseok0406/C20-모듈과-IOCP로-완성하는-MMORPG-서버-1.-환경설정)
- C++20 모듈 환경으로 전환 및 기본 환경 세팅
- CorePch 모듈화, 모듈 vs PCH 차이, 빌드 설정 방법
- GameServer 전용 Pch 구성

---

### 2️⃣ [ThreadManager](https://velog.io/@minseok0406/C20-모듈과-IOCP로-완성하는-MMORPG-서버-2.-ThreadManager)
- 네트워크 라이브러리 기반의 ThreadManager 설계
- CoreGlobal / CoreMacro / CoreTLS 구조
- C++20 `jthread`와 thread_local을 활용한 스레드 관리

---

### 3️⃣ [Reader-Writer Lock](https://velog.io/@minseok0406/C20-모듈과-IOCP로-완성하는-MMORPG-서버-3.-Reader-Writer-Lock)
- 원자적 CAS 기반 Reader-Writer Lock 구현
- WriteLock / ReadLock 동작 방식과 Unlock 처리
- RAII 기법을 적용한 LockGuard
- 매크로(`READ_LOCK`, `WRITE_LOCK`)를 통한 간편 적용

---

### 4️⃣ [CoreMacro 개선](https://velog.io/@minseok0406/C20-%EB%AA%A8%EB%93%88%EA%B3%BC-IOCP%EB%A1%9C-%EC%99%84%EC%84%B1%ED%95%98%EB%8A%94-MMORPG-%EC%84%9C%EB%B2%84-4.-CoreMacro-%EA%B0%9C%EC%84%A0)
- 함수형 매크로를 inline, constexpr 기반의 모듈 인터페이스로 개선
- `CRASH`, `ASSERT_CRASH` 매크로 → `noreturn` 함수로 대체, 컴파일러 최적화 유리
- `LockBox`, `READ_LOCK(_IDX)`, `WRITE_LOCK(_IDX)` 템플릿 제공: `nodiscard`, `source_location` 활용

---

### 5️⃣ [DeadLock 탐지](https://velog.io/@minseok0406/C20-모듈과-IOCP로-완성하는-MMORPG-서버-5.-DeadLock-%ED%83%90%EC%A7%80)
- DeadLock 탐지를 위한 프로파일러 구현
- Lock 획득 간의 관계를 그래프 구조로 기록, DFS로 순환 감지
- 순환 경로 발생 시 로그 출력 (C++20 `format` 사용) 및 `CRASH` 호출

---

### 6️⃣ [Allocator 개선](https://velog.io/@minseok0406/C20-%EB%AA%A8%EB%93%88%EA%B3%BC-IOCP%EB%A1%9C-%EC%99%84%EC%84%B1%ED%95%98%EB%8A%94-MMORPG-%EC%84%9C%EB%B2%84-6.-Allocator-%EA%B0%9C%EC%84%A0)
- 메모리 관리 구조 개선 및 `std::construct_at`, `std::destroy_at` 도입
- 기존 `placement new` 기반 코드를 현대 C++ 스타일로 대체
- `xnew`, `xdelete` 헬퍼 함수 제공 (CoreMacro 내 wrapper)
- 향후 STL 컨테이너(`vector`, `stack`, `queue`)의 커스텀 Allocator 기반 확장을 위한 기반 설계

---

## 🚀 마무리

이 시리즈는 C++20 모듈과 Windows IOCP를 활용해 MMORPG 서버를 설계하면서 겪은 문제와 해결 과정을 공유합니다.
앞으로도 지속적으로 업데이트될 예정이다.
