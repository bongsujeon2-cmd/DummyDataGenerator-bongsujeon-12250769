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
DummyDataGenerator.exe -s <schema.json> [-n <count>] [-o <output.json>] [-r]
```

| 옵션 | 설명 | 기본값 |
|------|------|--------|
| `-s <file>` | 스키마 JSON 파일 경로 **(필수)** | — |
| `-n <count>` | 생성할 레코드 수 | `10` |
| `-o <file>` | 출력 파일 경로 | `output.json` |
| `-r` | Raw Array 형식으로 출력 (기본: Repository 형식) | — |
| `-h` | 도움말 출력 | — |

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

`JsonRepository<T>`가 직접 읽을 수 있는 형식으로 출력합니다.

```json
{
  "nextId": 6,
  "entities": [
    { "id": 1, ... },
    { "id": 2, ... }
  ]
}
```

`id`와 `nextId`는 자동으로 순번 부여되며, 스키마에 선언하지 않아도 됩니다.

### Raw Array 형식 (`-r` 옵션)

```json
[
  { ... },
  { ... }
]
```

---

## JSON Schema 작성법

최상위는 반드시 `"type": "object"` 이어야 합니다.

### 지원 타입 및 키워드

| `type` | 사용 가능한 키워드 |
|--------|-------------------|
| `object` | `properties` |
| `array` | `items`, `minItems`, `maxItems` |
| `string` | `format`, `minLength`, `maxLength`, `enum` |
| `integer` | `minimum`, `maximum` |
| `number` | `minimum`, `maximum` |
| `boolean` | — |

### string `format` 목록

| format | 생성 예시 |
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
`enum`을 지정하면 목록 중 하나를 랜덤으로 선택합니다.

---

## 예제

### 예제 1 — User 스키마 (Repository 형식)

**schema_user.json**
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

**실행**
```
DummyDataGenerator.exe -s schema_user.json -n 5 -o users.json
```

**출력 (users.json)**
```json
{
  "entities": [
    {
      "age": 48,
      "email": "henry521@yahoo.com",
      "id": 1,
      "name": "Bob Robinson"
    },
    {
      "age": 39,
      "email": "diana622@test.org",
      "id": 2,
      "name": "Mia Brown"
    },
    {
      "age": 63,
      "email": "uma425@mail.net",
      "id": 3,
      "name": "Bob Robinson"
    },
    {
      "age": 48,
      "email": "victor650@example.com",
      "id": 4,
      "name": "Grace Robinson"
    },
    {
      "age": 33,
      "email": "zoe978@company.com",
      "id": 5,
      "name": "Bob Wilson"
    }
  ],
  "nextId": 6
}
```

---

### 예제 2 — Product 스키마 (Repository 형식)

**schema_product.json**
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

**실행**
```
DummyDataGenerator.exe -s schema_product.json -n 5 -o products.json
```

**출력 (products.json)**
```json
{
  "entities": [
    {
      "id": 1,
      "name": "Mega Device",
      "price": 499.64,
      "stock": 135
    },
    {
      "id": 2,
      "name": "Pro Series",
      "price": 408.36,
      "stock": 293
    },
    {
      "id": 3,
      "name": "Gadget Plus",
      "price": 747.33,
      "stock": 250
    },
    {
      "id": 4,
      "name": "Widget Pro",
      "price": 893.71,
      "stock": 112
    },
    {
      "id": 5,
      "name": "Power Unit",
      "price": 763.74,
      "stock": 450
    }
  ],
  "nextId": 6
}
```

---

### 예제 3 — 복합 스키마 (Raw Array 형식)

`boolean`, `enum`, 중첩 `array`, `number` 등 여러 타입을 한 번에 사용하는 예제입니다.

**schema_complex.json**
```json
{
  "type": "object",
  "properties": {
    "username": { "type": "string",  "format": "first-name" },
    "city":     { "type": "string",  "format": "city" },
    "score":    { "type": "number",  "minimum": 0.0,  "maximum": 100.0 },
    "active":   { "type": "boolean" },
    "level":    { "type": "integer", "minimum": 1, "maximum": 10 },
    "role":     { "type": "string",  "enum": ["admin", "user", "guest"] },
    "tags":     { "type": "array",   "items": { "type": "string", "format": "word" },
                  "minItems": 1, "maxItems": 3 }
  }
}
```

**실행**
```
DummyDataGenerator.exe -s schema_complex.json -n 3 -o result.json -r
```

**출력 (result.json)**
```json
[
  {
    "active": false,
    "city": "Rome",
    "level": 6,
    "role": "user",
    "score": 87.35,
    "tags": [
      "zeta",
      "gamma",
      "bold"
    ],
    "username": "Victor"
  },
  {
    "active": false,
    "city": "Tokyo",
    "level": 7,
    "role": "guest",
    "score": 7.05,
    "tags": [
      "swift",
      "epsilon"
    ],
    "username": "Iris"
  },
  {
    "active": false,
    "city": "Berlin",
    "level": 9,
    "role": "user",
    "score": 89.68,
    "tags": [
      "bold",
      "zeta"
    ],
    "username": "Mia"
  }
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
