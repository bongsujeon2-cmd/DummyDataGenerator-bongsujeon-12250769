# DummyDataGenerator

JSON Schema를 입력하면 스키마에 맞는 랜덤 더미 데이터를 지정한 개수만큼 생성하는 C++ 콘솔 도구입니다.  
출력 형식은 [DataMonitor](https://github.com/bongsujeon2-cmd/DataMonitor-bongsujeon-12250769)의 `JsonRepository`와 호환됩니다.

---

## 빌드

**요구 사항**: Visual Studio 2022 이상, C++20

Visual Studio에서 `DummyDataGenerator.sln`을 열거나, MSBuild로 직접 빌드합니다.

```
MSBuild DummyDataGenerator\DummyDataGenerator.vcxproj /p:Configuration=Release /p:Platform=x64
```

빌드 결과물: `DummyDataGenerator\x64\Release\DummyDataGenerator.exe`

---

## 사용법

### CLI 모드

```
DummyDataGenerator.exe -s <schema.json> -n <count> -o <output.json> [-r]
```

| 옵션 | 설명 | 기본값 |
|------|------|--------|
| `-s <file>` | 스키마 JSON 파일 경로 **(필수)** | - |
| `-n <count>` | 생성할 레코드 수 | `10` |
| `-o <file>` | 출력 파일 경로 | `output.json` |
| `-r` | Raw Array 형식으로 출력 (기본: Repository 형식) | - |
| `-h` | 도움말 출력 | - |

**예시**

```bash
# users.json 스키마로 50개 생성
DummyDataGenerator.exe -s schema_user.json -n 50 -o users.json

# Raw Array 형식으로 생성
DummyDataGenerator.exe -s schema_product.json -n 20 -o products.json -r
```

### 인터랙티브 메뉴

인수 없이 실행하면 메뉴가 열립니다.

```
DummyDataGenerator.exe
```

```
============================================================
  DummyDataGenerator  v1.0
============================================================
  Schema : (not loaded)
  Count  : 10
  Output : output.json
  Format : Repository / DataMonitor

  [1] Load schema from file
  [2] Set record count
  [3] Set output file
  [4] Toggle output format
  [5] Preview loaded schema
  [6] Generate!
  [0] Exit
```

---

## 출력 형식

### Repository 형식 (기본 — DataMonitor 호환)

`JsonRepository`가 직접 읽을 수 있는 형식으로 출력합니다.

```json
{
  "nextId": 4,
  "entities": [
    { "id": 1, "name": "Alice Johnson", "email": "alice42@gmail.com", "age": 31 },
    { "id": 2, "name": "Bob Smith",     "email": "bob17@example.com", "age": 45 },
    { "id": 3, "name": "Eve Williams",  "email": "eve99@yahoo.com",   "age": 27 }
  ]
}
```

### Raw Array 형식 (`-r` 옵션)

```json
[
  { "name": "Widget Pro", "price": 149.99, "stock": 120 },
  { "name": "Core Kit",   "price": 39.50,  "stock": 8 }
]
```

---

## JSON Schema 작성법

최상위는 반드시 `type: object`이어야 합니다.  
`id` 필드는 Repository 형식 출력 시 자동으로 1부터 순번 부여되므로 스키마에 선언하지 않아도 됩니다.

### 지원 타입

| `type` | 추가 키워드 |
|--------|-------------|
| `object` | `properties` |
| `array` | `items`, `minItems`, `maxItems` |
| `string` | `format`, `minLength`, `maxLength`, `enum` |
| `integer` | `minimum`, `maximum` |
| `number` | `minimum`, `maximum` |
| `boolean` | — |

### string format 목록

| format | 예시 출력 |
|--------|-----------|
| `email` | `alice42@gmail.com` |
| `name` | `Alice Johnson` |
| `first-name` | `Alice` |
| `last-name` | `Johnson` |
| `product` | `Widget Pro` |
| `city` | `Seoul` |
| `sentence` | `fast blue new alpha bold` |
| `word` | `swift` |

`format` 없이 `minLength` / `maxLength`만 지정하면 해당 길이의 랜덤 단어를 생성합니다.

---

## 예제 스키마

### schema_user.json

```json
{
  "type": "object",
  "properties": {
    "name":  { "type": "string",  "format": "name" },
    "email": { "type": "string",  "format": "email" },
    "age":   { "type": "integer", "minimum": 18, "maximum": 65 }
  }
}
```

실행:
```
DummyDataGenerator.exe -s schema_user.json -n 10 -o users.json
```

출력 (`users.json`):
```json
{
  "nextId": 11,
  "entities": [
    { "id": 1, "age": 57, "email": "bob243@webmail.io",   "name": "Olivia Martin" },
    { "id": 2, "age": 20, "email": "frank991@gmail.com",  "name": "Victor Martinez" },
    ...
  ]
}
```

### schema_product.json

```json
{
  "type": "object",
  "properties": {
    "name":  { "type": "string",  "format": "product" },
    "price": { "type": "number",  "minimum": 1.0, "maximum": 999.99 },
    "stock": { "type": "integer", "minimum": 0,   "maximum": 500 }
  }
}
```

### schema_complex.json — 복합 타입 예제

```json
{
  "type": "object",
  "properties": {
    "username": { "type": "string",  "format": "first-name" },
    "city":     { "type": "string",  "format": "city" },
    "score":    { "type": "number",  "minimum": 0.0, "maximum": 100.0 },
    "active":   { "type": "boolean" },
    "level":    { "type": "integer", "minimum": 1, "maximum": 10 },
    "role":     { "type": "string",  "enum": ["admin", "user", "guest"] },
    "tags":     { "type": "array",   "items": { "type": "string", "format": "word" },
                  "minItems": 1, "maxItems": 3 }
  }
}
```

실행:
```
DummyDataGenerator.exe -s schema_complex.json -n 3 -r
```

출력:
```json
[
  { "active": false, "city": "Tokyo",   "level": 4, "role": "admin", "score": 11.85, "tags": ["beta"],           "username": "Bob" },
  { "active": false, "city": "Berlin",  "level": 3, "role": "admin", "score": 93.9,  "tags": ["swift", "new"],  "username": "Eve" },
  { "active": true,  "city": "Toronto", "level": 1, "role": "user",  "score": 36.66, "tags": ["swift", "blue"], "username": "Iris" }
]
```

---

## 파일 구성

```
DummyDataGenerator/
├── json.hpp               경량 JSON 파서 (DataMonitor와 공유)
├── SchemaGenerator.h      스키마 기반 랜덤 데이터 생성 엔진
├── main.cpp               CLI 및 인터랙티브 메뉴 진입점
├── schema_user.json       예제 스키마 — User
├── schema_product.json    예제 스키마 — Product
└── schema_complex.json    예제 스키마 — 복합 타입
```

---

## DataMonitor 연동

생성된 Repository 형식 파일은 DataMonitor의 `JsonRepository<T>`가 그대로 읽을 수 있습니다.

1. DummyDataGenerator로 `users.json` 생성
2. 파일을 DataMonitor 실행 경로에 복사
3. DataMonitor 실행 → Users Management → List All 로 데이터 확인
