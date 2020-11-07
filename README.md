# Lisp Interpreter Specification
프로그래밍언어론 팀플

## 1. Lisp의 산술연산

**LISP** 에서의 연산은 모두 함수적용에 의하여 이루어진다. **operator**가 앞에 오고 그 뒤에 **operand** 혹은 **Argument**가 오는 **Prefix Notation**으로 사용한다. 
**;**은 생략가능하고, **;** 다음엔 **comment**가 나올 수 있다, 영문 대소문자는 구분없시 처리한다.

 - **사칙 연산(+, -, *, / )**
다음은 3+5의 계산과 (3+(5*6))의 계산이다.
```cpp
>(+ 3 5)
8
```
```cpp
> (+ 3 (* 5 6))
33
```


## 1. Lisp의 기본함수




## 1. Lisp의 Predicate함수



## Lisp의 조건문
