# TODO

했으면 changelog에다가 기재할것

Done:
- 텍스트 렌더링 최적화
- json api
- 엔티티 기본 클래스
- 장비 기본 클래스
- 맵 이미지 기반으로 다시 고치기
- 엔티티/ 팀 생성-삭제 동작

-README:
    - 프로젝트 아키텍처 설명 첨부: in progress

- 리팩토링
    - 버튼 클래스: not yet

- 시스템:
    - 랜덤 클래스 구현: not yet
    - 실수, 정수, 부울 랜더마이저를 구분: not yet

- 그래픽:
    - 디버그용 엔티티 스프라이트: done
    - 히트 스프라이트 구현: in progress
- 그래픽 최적화:
    - 맵 타일이 각자 텍스처 객체를 갖는걸 수정: not yet
    - 정확히 말하면 플라이웨이트 디자인 패턴을 씀.

- 데이터
    - 유저 데이터 세이브/로드: in progress

- 시나리오:
    - 시나리오 클래스 구현: done
    - 동작: 
        - 조건에 충족되면 다음 시나리오로 넘어가는 옵저버 패턴 구현: in progress
    - 데이터:
        - 시나리오 데이터 로딩 동작: done
        - 인트로 부분 구현: in progress
        - 메인 시나리오, 서브 시나리오 구분: in progress

- 맵
    - 카메라 구현: done
    - 줌인 줌아웃 구현: maybe?

- 맵 기본 ui
    - 맵 툴팁 구현: done
    - 맵 객체에 존재하는 다른 정보 로드: done
    - 타일 객체의 정보 컨테이너 클린업 기능: done

- 서브 맵:
    - 전투/탐험용 서브맵 구현: done
    - 서브맵 지형 구현: in progress
    - 오버맵으로 이동 기능 구현: in progress

- 맵 이동:
    - 엔티티 이동시 연관 데이터도 움직이게 바꿈: done
    - 엔티티 이동시 행동력을 소모하게 바꿈: in progress 

- 전투 시스템:
    - 서브맵 턴 구현: done
    - 엔티티 포커스 구현: done
    - 근접전 시스템 구현: in progress
    - 원거리전 시스템 구현: in progress
    - 엄폐 시스템 구현: in progress
    - 범위 공격 구현: in progress

- 전투 ui:
    - 엔티티 정보 확인 팝업 구현: in progress
    - 공격(이동) 범위 표시 구현: done
    - 체력바, ap바 등 간략화된 엔티티 정보 ui 구현: in progress

- 스킬:
    - 스킬 데이터 구현: in progress
    - 스킬 로직 구현: in progress
    - 엔티티에 실제로 연결: in progress
    - ui에도 표시: in progress

- 엔티티:
    - 데이터:
        - 엔티티 데이터를 개별 엔티티로 쪼개지 말고 카테고리로만 구현하고 실제 로직에서 플레이어 레벨에 맞게 스케일링하여 엔티티 객체 구현: in progress
        
        - 엔티티 데이터에서 hp 같은 스탯을 그대로 가져오지 않고, str 등과 같은 스탯을 가져와서 로직에 따라 실스탯을 결정하게 만듬: in progress
    
    - pc(플레이어블 캐릭터) 특성 클래스 : not yet

- 아이템:
    - 장비:
        - 장비 클래스 구현: done
        - 장비 데이터베이스 구현: done
        - 전투 시스템에 장비 데이터 반영: yet
        - ui에도 구현: yet

- 도시 기본 ui
    - ui 클래스와 맵 클래스 연결(ui manager): done
    - 툴팁: done

- 텍스트
    - 스스로의 길이를 구하는 메서드: done

- 범용 ui
    - 텍스트 아틀라스 구현: maybe
    - 툴팁
        - 툴팁 동적 렌더링: done
        - 툴팁 깜박임 해결: done
        
    - 턴 종료 버튼 구현: done
    - 턴 종료 버튼 기능 완성: done
    - 로그 ui 구현: in progress

- 대화 ui
    - 비주얼 구현: done
    - 기본 작동: done



