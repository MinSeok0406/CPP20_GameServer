# C++20 모듈과 IOCP로 완성하는 MMORPG 서버

이 저장소는 Velog 블로그 시리즈 **C++20 & IOCP**를 기반으로 작성되었습니다.  
개발 과정에서 겪었던 고민과 시행착오, 그리고 문제 해결 과정을 공유합니다.

---

## 📖 시리즈 링크

- [Velog 시리즈 전체 보기](https://velog.io/@minseok0406/series/C20-IOCP)

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

## 🚀 마무리

이 시리즈는 C++20 모듈과 Windows IOCP를 활용해 MMORPG 서버를 설계하면서 겪은 문제와 해결 과정을 공유합니다.
앞으로도 지속적으로 업데이트될 예정이다.
